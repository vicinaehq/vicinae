#include "windows-update-installer.hpp"
#include "semver.hpp"
#include "update-service.hpp"
#include "vicinae.hpp"
#include <common/common.hpp>
#include <QCoreApplication>
#include <QDebug>
#include <QProcess>
#include <QSettings>
#include <QThreadPool>
#include <windows.h>
#include <softpub.h>
#include <wincrypt.h>
#include <wintrust.h>
#include <format>
#include <system_error>

namespace fs = std::filesystem;

namespace {

// Must match AppId in extra/windows/vicinae.iss (Inno appends _is1 to the uninstall key)
constexpr const char *INNO_UNINSTALL_KEY =
    R"(HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Uninstall\{C698C8E4-B6C9-4C86-A9AA-520A6D2E45A1}_is1)";
constexpr const char *INSTALL_LOG_NAME = "update-install.log";

std::optional<fs::path> innoInstallLocation() {
  QSettings reg(INNO_UNINSTALL_KEY, QSettings::NativeFormat);
  const QString location = reg.value("InstallLocation").toString();

  if (location.isEmpty()) return std::nullopt;

  return fs::path(location.toStdString());
}

std::optional<WindowsUpdateInstaller::Signer> readSigner(const fs::path &file) {
  HCERTSTORE store = nullptr;
  HCRYPTMSG msg = nullptr;
  DWORD encoding = 0;

  if (!CryptQueryObject(CERT_QUERY_OBJECT_FILE, file.c_str(), CERT_QUERY_CONTENT_FLAG_PKCS7_SIGNED_EMBED,
                        CERT_QUERY_FORMAT_FLAG_BINARY, 0, &encoding, nullptr, nullptr, &store, &msg,
                        nullptr)) {
    return std::nullopt;
  }

  std::optional<WindowsUpdateInstaller::Signer> signer;
  DWORD size = 0;

  if (CryptMsgGetParam(msg, CMSG_SIGNER_INFO_PARAM, 0, nullptr, &size)) {
    std::vector<BYTE> buf(size);

    if (CryptMsgGetParam(msg, CMSG_SIGNER_INFO_PARAM, 0, buf.data(), &size)) {
      auto *info = reinterpret_cast<CMSG_SIGNER_INFO *>(buf.data());
      CERT_INFO certInfo{};

      certInfo.Issuer = info->Issuer;
      certInfo.SerialNumber = info->SerialNumber;

      if (PCCERT_CONTEXT cert =
              CertFindCertificateInStore(store, encoding, 0, CERT_FIND_SUBJECT_CERT, &certInfo, nullptr)) {
        const DWORD len =
            CertNameToStrW(X509_ASN_ENCODING, &cert->pCertInfo->Subject, CERT_X500_NAME_STR, nullptr, 0);
        std::wstring subject(len, L'\0');

        CertNameToStrW(X509_ASN_ENCODING, &cert->pCertInfo->Subject, CERT_X500_NAME_STR, subject.data(), len);
        subject.resize(len ? len - 1 : 0);

        DWORD hashLen = 0;
        CertGetCertificateContextProperty(cert, CERT_SHA1_HASH_PROP_ID, nullptr, &hashLen);
        std::vector<unsigned char> thumbprint(hashLen);
        CertGetCertificateContextProperty(cert, CERT_SHA1_HASH_PROP_ID, thumbprint.data(), &hashLen);

        if (!subject.empty() && hashLen > 0) {
          signer = WindowsUpdateInstaller::Signer{.subject = std::move(subject),
                                                  .thumbprint = std::move(thumbprint)};
        }

        CertFreeCertificateContext(cert);
      }
    }
  }

  CryptMsgClose(msg);
  CertCloseStore(store, 0);

  return signer;
}

LONG verifyTrust(const fs::path &file) {
  WINTRUST_FILE_INFO fileInfo{};
  fileInfo.cbStruct = sizeof(fileInfo);
  fileInfo.pcwszFilePath = file.c_str();

  WINTRUST_DATA data{};
  data.cbStruct = sizeof(data);
  data.dwUIChoice = WTD_UI_NONE;
  data.fdwRevocationChecks = WTD_REVOKE_NONE;
  data.dwUnionChoice = WTD_CHOICE_FILE;
  data.pFile = &fileInfo;
  data.dwStateAction = WTD_STATEACTION_VERIFY;
  data.dwProvFlags = WTD_CACHE_ONLY_URL_RETRIEVAL;

  GUID action = WINTRUST_ACTION_GENERIC_VERIFY_V2;
  const LONG status = WinVerifyTrust(static_cast<HWND>(INVALID_HANDLE_VALUE), &action, &data);

  data.dwStateAction = WTD_STATEACTION_CLOSE;
  WinVerifyTrust(static_cast<HWND>(INVALID_HANDLE_VALUE), &action, &data);

  return status;
}

std::optional<vicinae::Semver> fileVersion(const fs::path &file) {
  DWORD handle = 0;
  const DWORD size = GetFileVersionInfoSizeW(file.c_str(), &handle);

  if (!size) return std::nullopt;

  std::vector<BYTE> buf(size);

  if (!GetFileVersionInfoW(file.c_str(), 0, size, buf.data())) return std::nullopt;

  VS_FIXEDFILEINFO *info = nullptr;
  UINT len = 0;

  if (!VerQueryValueW(buf.data(), L"\\", reinterpret_cast<LPVOID *>(&info), &len) || !info) {
    return std::nullopt;
  }

  return vicinae::Semver{.components = {HIWORD(info->dwFileVersionMS), LOWORD(info->dwFileVersionMS),
                                        HIWORD(info->dwFileVersionLS), LOWORD(info->dwFileVersionLS)}};
}

QString formatVersion(const vicinae::Semver &version) {
  QString text;

  for (size_t i = 0; i < version.components.size(); ++i) {
    if (i) text += '.';
    text += QString::number(version.components[i]);
  }

  return text;
}

void sweepDownloads() {
  std::error_code ec;

  for (const auto &entry : fs::directory_iterator(UpdateService::downloadDir(), ec)) {
    fs::remove(entry.path(), ec);
  }
}

} // namespace

WindowsUpdateInstaller::WindowsUpdateInstaller() {
  const fs::path exe = vicinae::selfPath();
  const auto installLocation = innoInstallLocation();
  std::error_code ec;

  if (!installLocation || !fs::equivalent(*installLocation, exe.parent_path().parent_path(), ec)) return;

  auto signer = readSigner(exe);

  if (!signer) {
    qInfo() << "Running binary is not signed, self update is disabled";
    return;
  }

  m_self = std::move(signer);

  sweepDownloads();
}

QString WindowsUpdateInstaller::assetName() const {
#if defined(_M_ARM64)
  return QStringLiteral("vicinae-arm64-setup.exe");
#else
  return QStringLiteral("vicinae-x64-setup.exe");
#endif
}

void WindowsUpdateInstaller::install(const std::filesystem::path &archive, const QString &expectedVersion) {
  if (!m_self) {
    emit failed(tr("This installation cannot update itself"));
    return;
  }

  QThreadPool::globalInstance()->start(
      [this, archive, expectedVersion]() { performInstall(archive, expectedVersion); });
}

std::optional<QString> WindowsUpdateInstaller::verifySignature(const std::filesystem::path &setup) const {
  const auto signer = readSigner(setup);

  if (!signer) return tr("The update is not signed");

  const LONG status = verifyTrust(setup);

  // A certificate identical to our own is trusted even without a chain, so builds
  // signed with a self-signed development certificate can update themselves.
  if (status == CERT_E_UNTRUSTEDROOT && signer->thumbprint == m_self->thumbprint) return std::nullopt;

  if (status != ERROR_SUCCESS) {
    return tr("Update signature verification failed (0x%1)")
        .arg(static_cast<quint32>(status), 8, 16, QChar('0'));
  }

  if (signer->subject != m_self->subject) {
    return tr("Update is signed by %1, expected %2")
        .arg(QString::fromStdWString(signer->subject))
        .arg(QString::fromStdWString(m_self->subject));
  }

  return std::nullopt;
}

void WindowsUpdateInstaller::performInstall(const std::filesystem::path &archive,
                                            const QString &expectedVersion) {
  emit stageChanged(tr("Verifying update…"));

  if (auto error = verifySignature(archive)) {
    emit failed(*error);
    return;
  }

  const auto version = fileVersion(archive);
  const auto expected = vicinae::Semver::parse(expectedVersion.toStdString());

  if (!version) {
    emit failed(tr("Update has no version information"));
    return;
  }

  if (!expected || *version != *expected) {
    emit failed(tr("Update version mismatch: expected %1, found %2")
                    .arg(expectedVersion)
                    .arg(formatVersion(*version)));
    return;
  }

  emit stageChanged(tr("Starting installer…"));

  const fs::path log = Omnicast::stateDir() / INSTALL_LOG_NAME;
  const QStringList args = {"/VERYSILENT",   "/SUPPRESSMSGBOXES",
                            "/NORESTART",    "/NOCANCEL",
                            "/AUTOUPDATE=1", QString::fromStdString(std::format("/LOG={}", log.string()))};

  if (!QProcess::startDetached(QString::fromStdString(archive.string()), args)) {
    emit failed(tr("Failed to start the installer"));
    return;
  }

  emit finished();
}

// The installer waits for this process to exit, replaces the files and
// relaunches the server through its /AUTOUPDATE [Run] entry
void WindowsUpdateInstaller::relaunch() { QCoreApplication::quit(); }
