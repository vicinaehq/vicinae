import CryptoKit
import Foundation

@_cdecl("vicinae_random_bytes")
func vicinae_random_bytes(_ out: UnsafeMutablePointer<UInt8>, _ len: Int) -> Bool {
  if len == 0 { return true }
  let key = SymmetricKey(size: SymmetricKeySize(bitCount: len * 8))
  key.withUnsafeBytes { raw in
    out.update(from: raw.bindMemory(to: UInt8.self).baseAddress!, count: len)
  }
  return true
}

@_cdecl("vicinae_hkdf")
func vicinae_hkdf(
  _ keyPtr: UnsafePointer<UInt8>, _ keyLen: Int,
  _ saltPtr: UnsafePointer<UInt8>?, _ saltLen: Int,
  _ infoPtr: UnsafePointer<UInt8>?, _ infoLen: Int,
  _ out: UnsafeMutablePointer<UInt8>, _ outLen: Int
) -> Bool {
  let ikm = SymmetricKey(data: Data(bytes: keyPtr, count: keyLen))
  let salt = saltLen > 0 ? Data(bytes: saltPtr!, count: saltLen) : Data()
  let info = infoLen > 0 ? Data(bytes: infoPtr!, count: infoLen) : Data()
  let derived = HKDF<SHA256>.deriveKey(inputKeyMaterial: ikm, salt: salt, info: info, outputByteCount: outLen)
  derived.withUnsafeBytes { raw in
    out.update(from: raw.bindMemory(to: UInt8.self).baseAddress!, count: outLen)
  }
  return true
}

@_cdecl("vicinae_gcm_encrypt")
func vicinae_gcm_encrypt(
  _ keyPtr: UnsafePointer<UInt8>, _ keyLen: Int,
  _ ivPtr: UnsafePointer<UInt8>, _ ivLen: Int,
  _ ptPtr: UnsafePointer<UInt8>?, _ ptLen: Int,
  _ ctOut: UnsafeMutablePointer<UInt8>?,
  _ tagOut: UnsafeMutablePointer<UInt8>, _ tagLen: Int
) -> Bool {
  let key = SymmetricKey(data: Data(bytes: keyPtr, count: keyLen))
  guard let nonce = try? AES.GCM.Nonce(data: Data(bytes: ivPtr, count: ivLen)) else { return false }
  let plaintext = ptLen > 0 ? Data(bytes: ptPtr!, count: ptLen) : Data()
  guard let box = try? AES.GCM.seal(plaintext, using: key, nonce: nonce) else { return false }
  guard box.tag.count == tagLen else { return false }
  box.tag.copyBytes(to: tagOut, count: tagLen)
  if ptLen > 0 {
    guard let ctOut, box.ciphertext.count == ptLen else { return false }
    box.ciphertext.copyBytes(to: ctOut, count: ptLen)
  }
  return true
}

@_cdecl("vicinae_gcm_decrypt")
func vicinae_gcm_decrypt(
  _ keyPtr: UnsafePointer<UInt8>, _ keyLen: Int,
  _ ivPtr: UnsafePointer<UInt8>, _ ivLen: Int,
  _ ctPtr: UnsafePointer<UInt8>?, _ ctLen: Int,
  _ tagPtr: UnsafePointer<UInt8>, _ tagLen: Int,
  _ ptOut: UnsafeMutablePointer<UInt8>?
) -> Bool {
  let key = SymmetricKey(data: Data(bytes: keyPtr, count: keyLen))
  guard let nonce = try? AES.GCM.Nonce(data: Data(bytes: ivPtr, count: ivLen)) else { return false }
  let ciphertext = ctLen > 0 ? Data(bytes: ctPtr!, count: ctLen) : Data()
  let tag = Data(bytes: tagPtr, count: tagLen)
  guard let box = try? AES.GCM.SealedBox(nonce: nonce, ciphertext: ciphertext, tag: tag) else { return false }
  guard let plaintext = try? AES.GCM.open(box, using: key) else { return false }
  if ctLen > 0 {
    guard let ptOut, plaintext.count == ctLen else { return false }
    plaintext.copyBytes(to: ptOut, count: ctLen)
  }
  return true
}
