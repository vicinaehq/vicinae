<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="ko">
<context>
    <name>AI::AcpProvider</name>
    <message>
        <location filename="../src/services/ai/acp/acp-provider.cpp" line="+548"/>
        <source>Not installed</source>
        <translation>설치되지 않음</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Requires Node.js</source>
        <translation>Node.js 필요</translation>
    </message>
    <message>
        <location line="+19"/>
        <source>Not signed in. Run &quot;%1&quot; in a terminal to sign in.</source>
        <translation>로그인하지 않았습니다. 터미널에서 &quot;%1&quot; 명령을 실행해 로그인하세요.</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Not signed in. Sign in to the agent from a terminal.</source>
        <translation>로그인하지 않았습니다. 터미널에서 에이전트에 로그인하세요.</translation>
    </message>
    <message>
        <location line="+32"/>
        <source>Starting %1…</source>
        <translation>%1 시작 중…</translation>
    </message>
    <message>
        <location line="+20"/>
        <location line="+98"/>
        <source>Could not start %1.</source>
        <translation>%1을 시작할 수 없습니다.</translation>
    </message>
    <message>
        <location line="-74"/>
        <location line="+9"/>
        <source>No command is set.</source>
        <translation>설정된 명령이 없습니다.</translation>
    </message>
    <message>
        <location line="-5"/>
        <source>Could not find %1.</source>
        <translation>%1을 찾을 수 없습니다.</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Requires Node.js to run %1, or %2 to be installed.</source>
        <translation>%1을 실행하려면 Node.js가 있거나 %2이 설치되어 있어야 합니다.</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>%1 is not installed, or could not be found.</source>
        <translation>%1이 설치되지 않았거나 찾을 수 없습니다.</translation>
    </message>
    <message>
        <location line="+76"/>
        <source>Timed out waiting for %1 to respond.</source>
        <translation>%1의 응답 대기 시간이 초과되었습니다.</translation>
    </message>
    <message>
        <location line="+175"/>
        <source>The agent was restarted, send your message again.</source>
        <translation>에이전트가 다시 시작되었습니다. 메시지를 다시 보내세요.</translation>
    </message>
    <message>
        <location line="+70"/>
        <source>The agent declined to respond.</source>
        <translation>에이전트가 응답을 거부했습니다.</translation>
    </message>
</context>
<context>
    <name>AI::Agent</name>
    <message>
        <location filename="../src/services/ai/agentic-loop.cpp" line="+210"/>
        <source>The agent reached its step limit.</source>
        <translation>에이전트가 최대 단계 수에 도달했습니다.</translation>
    </message>
    <message>
        <location line="+37"/>
        <source>This model is not available right now.</source>
        <translation>현재 이 모델을 사용할 수 없습니다.</translation>
    </message>
    <message>
        <location line="+50"/>
        <source>Could not start the response.</source>
        <translation>응답을 시작할 수 없습니다.</translation>
    </message>
    <message>
        <location line="+27"/>
        <source>The model returned an invalid tool call.</source>
        <translation>모델이 잘못된 도구 호출을 반환했습니다.</translation>
    </message>
    <message>
        <location line="+139"/>
        <location line="+187"/>
        <source>Tool execution was cancelled.</source>
        <translation>도구 실행이 취소되었습니다.</translation>
    </message>
    <message>
        <location line="-165"/>
        <location line="+60"/>
        <source>This tool is not enabled.</source>
        <translation>이 도구는 활성화되어 있지 않습니다.</translation>
    </message>
    <message>
        <location line="-14"/>
        <location line="+17"/>
        <source>The user declined to run this tool.</source>
        <translation>사용자가 이 도구의 실행을 거부했습니다.</translation>
    </message>
    <message>
        <location line="+49"/>
        <source>The tool did not return a result.</source>
        <translation>도구가 결과를 반환하지 않았습니다.</translation>
    </message>
</context>
<context>
    <name>AI::LocalProvider</name>
    <message>
        <location filename="../src/services/ai/local/local-provider.cpp" line="+22"/>
        <source>Vicinae-managed models, mostly for dictation purposes.</source>
        <translation>주로 음성 입력에 사용되는 Vicinae 관리 모델입니다.</translation>
    </message>
    <message>
        <location filename="../src/services/ai/local/local-provider.hpp" line="+26"/>
        <source>Built-in</source>
        <translation>내장</translation>
    </message>
</context>
<context>
    <name>AIChatCommand</name>
    <message>
        <location filename="../src/builtins/ai/ai-extension.hpp" line="+28"/>
        <source>AI Chat</source>
        <translation>AI Chat</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Open your AI conversations in a dedicated window.</source>
        <translation>별도 창에서 AI 대화를 여세요.</translation>
    </message>
</context>
<context>
    <name>AIModelDeleteDialog</name>
    <message>
        <location filename="../src/ui/qml/settings/AIModelDeleteDialog.qml" line="+28"/>
        <source>Delete %1?</source>
        <translation>%1을 삭제할까요?</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>The model file will be removed from disk. You can download it again at any time.</source>
        <translation>모델 파일이 디스크에서 삭제됩니다. 언제든 다시 다운로드할 수 있습니다.</translation>
    </message>
    <message>
        <location line="+16"/>
        <source>Cancel</source>
        <translation>취소</translation>
    </message>
    <message>
        <location line="+12"/>
        <source>Delete</source>
        <translation>삭제</translation>
    </message>
</context>
<context>
    <name>AIModelRow</name>
    <message>
        <location filename="../src/ui/qml/settings/AIModelRow.qml" line="+106"/>
        <source>Download</source>
        <translation>다운로드</translation>
    </message>
    <message>
        <location line="+12"/>
        <source>Cancel</source>
        <translation>취소</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Downloading</source>
        <translation>다운로드 중</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>%1%</source>
        <translation>%1%</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>Delete model file</source>
        <translation>모델 파일 삭제</translation>
    </message>
</context>
<context>
    <name>AIProviderErrorDialog</name>
    <message>
        <location filename="../src/ui/qml/settings/AIProviderErrorDialog.qml" line="+69"/>
        <source>Copied!</source>
        <translation>복사했습니다!</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Copy</source>
        <translation>복사</translation>
    </message>
    <message>
        <location line="+19"/>
        <source>Close</source>
        <translation>닫기</translation>
    </message>
</context>
<context>
    <name>AIProviderPicker</name>
    <message>
        <location filename="../src/ui/qml/settings/AIProviderPicker.qml" line="+123"/>
        <source>Search providers…</source>
        <translation>제공자 검색…</translation>
    </message>
    <message>
        <location line="+44"/>
        <source>No providers match.</source>
        <translation>일치하는 제공자가 없습니다.</translation>
    </message>
    <message>
        <location line="+104"/>
        <source>Add another</source>
        <translation>추가하기</translation>
    </message>
</context>
<context>
    <name>AIProviderSetupDialog</name>
    <message>
        <location filename="../src/ui/qml/settings/AIProviderSetupDialog.qml" line="+86"/>
        <source>Set up %1</source>
        <translation>%1 설정</translation>
    </message>
    <message>
        <location line="+15"/>
        <source>Instance ID</source>
        <translation>인스턴스 ID</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>A unique identifier for this instance.</source>
        <translation>이 인스턴스의 고유 식별자입니다.</translation>
    </message>
    <message>
        <location line="+14"/>
        <source>This instance ID is already in use.</source>
        <translation>이미 사용 중인 인스턴스 ID입니다.</translation>
    </message>
    <message>
        <location line="+104"/>
        <source>Cancel</source>
        <translation>취소</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Add</source>
        <translation>추가</translation>
    </message>
</context>
<context>
    <name>AIProviderView</name>
    <message>
        <location filename="../src/ui/qml/settings/AIProviderView.qml" line="+108"/>
        <source>Providers</source>
        <translation>제공자</translation>
    </message>
    <message>
        <location line="+39"/>
        <source>Remove</source>
        <translation>제거</translation>
    </message>
    <message>
        <location line="+76"/>
        <source>Configuration</source>
        <translation>구성</translation>
    </message>
    <message>
        <location line="+146"/>
        <source>Details</source>
        <translation>세부 정보</translation>
    </message>
    <message>
        <location line="+23"/>
        <source>Models</source>
        <translation>모델</translation>
    </message>
    <message>
        <location line="+27"/>
        <source>No models available. Check the connection and configuration.</source>
        <translation>사용 가능한 모델이 없습니다. 연결과 설정을 확인하세요.</translation>
    </message>
</context>
<context>
    <name>AIProvidersOverview</name>
    <message>
        <location filename="../src/ui/qml/settings/AIProvidersOverview.qml" line="+183"/>
        <source>Your providers</source>
        <translation>내 제공자</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Add provider</source>
        <translation>제공자 추가</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Models from every provider you set up are available to all AI features, including dictation.</source>
        <translation>설정한 모든 제공자의 모델을 음성 입력을 포함한 모든 AI 기능에서 사용할 수 있습니다.</translation>
    </message>
    <message>
        <location line="+37"/>
        <source>API keys are stored unencrypted. To protect them, turn on Encrypt sensitive data in the Advanced settings.</source>
        <translation>API 키는 암호화되지 않은 상태로 저장됩니다. 키를 보호하려면 고급 설정에서 &apos;민감한 데이터 암호화&apos;를 켜세요.</translation>
    </message>
    <message>
        <location line="+15"/>
        <source>No providers yet. Add one to start using AI features.</source>
        <translation>아직 제공자가 없습니다. 제공자를 추가해 AI 기능을 사용하세요.</translation>
    </message>
</context>
<context>
    <name>AISettingsModel</name>
    <message>
        <location filename="../src/ui/settings/ai-settings-model.cpp" line="+73"/>
        <source>Built-in</source>
        <translation>내장</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Cloud APIs</source>
        <translation>클라우드 API</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Self-hosted</source>
        <translation>자체 호스팅</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Agents</source>
        <translation>에이전트</translation>
    </message>
    <message>
        <location line="+283"/>
        <source>No models installed</source>
        <translation>설치된 모델 없음</translation>
    </message>
    <message numerus="yes">
        <location line="+1"/>
        <source>%n model(s) installed</source>
        <translation>
            <numerusform>모델 %n개 설치됨</numerusform>
        </translation>
    </message>
    <message>
        <location line="+5"/>
        <source>No models found</source>
        <translation>모델을 찾을 수 없음</translation>
    </message>
    <message numerus="yes">
        <location line="+2"/>
        <source>%n model(s)</source>
        <translation>
            <numerusform>모델 %n개</numerusform>
        </translation>
    </message>
    <message>
        <location line="+123"/>
        <location line="+90"/>
        <source>Not connected</source>
        <translation>연결되지 않음</translation>
    </message>
</context>
<context>
    <name>AboutSettingsPage</name>
    <message>
        <location filename="../src/ui/qml/settings/AboutSettingsPage.qml" line="+61"/>
        <source>Version %1 - Commit %2
(%3)</source>
        <translation>버전 %1 - 커밋 %2
(%3)</translation>
    </message>
    <message>
        <location line="+24"/>
        <source>Documentation</source>
        <translation>문서</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Report a Bug</source>
        <translation>버그 신고</translation>
    </message>
</context>
<context>
    <name>ActionListPanel</name>
    <message>
        <location filename="../src/ui/qml/actions/ActionListPanel.qml" line="+116"/>
        <source>No matching actions</source>
        <translation>일치하는 동작 없음</translation>
    </message>
    <message>
        <location line="+136"/>
        <source>Filter actions...</source>
        <translation>동작 필터링...</translation>
    </message>
</context>
<context>
    <name>AddVocabCommand</name>
    <message>
        <location filename="../src/builtins/dictation/dictation-extension.hpp" line="+27"/>
        <source>Add Word to Vocabulary</source>
        <translation>단어장에 단어 추가</translation>
    </message>
</context>
<context>
    <name>AdvancedSettingsPage</name>
    <message>
        <location filename="../src/ui/qml/settings/AdvancedSettingsPage.qml" line="+35"/>
        <source>Input &amp; Navigation</source>
        <translation>입력 및 탐색</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Pop on backspace</source>
        <translation>백스페이스로 뒤로 이동</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Pop back in navigation on backspace when no input is present.</source>
        <translation>입력한 내용이 없을 때 백스페이스를 누르면 이전 화면으로 돌아갑니다.</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Activate on single click</source>
        <translation>한 번 클릭으로 실행</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Activate items with a single click instead of requiring a double click.</source>
        <translation>두 번 클릭할 필요 없이 한 번 클릭하면 항목을 실행합니다.</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Wrap navigation</source>
        <translation>목록 순환</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Wrap around to the opposite end when moving past the first or last item.</source>
        <translation>첫 항목이나 마지막 항목을 지나면 반대쪽 끝으로 이동합니다.</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>IME handling</source>
        <translation>입력기 처리</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Include IME Preedit strings as part of search queries.</source>
        <translation>입력기에서 조합 중인 문자열을 검색어에 포함합니다.</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Keybinding Scheme</source>
        <translation>키 바인딩 방식</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Default uses the standard macOS keys (arrows, Ctrl+N/P); Vim uses Ctrl+J/K and Ctrl+H/L; Emacs uses Ctrl+N/P and Ctrl+Opt+B/F for navigation, plus Emacs editing in the search bar.</source>
        <translation>기본 방식은 macOS 표준 키(방향키, Ctrl+N/P)를 사용합니다. Vim은 Ctrl+J/K 및 Ctrl+H/L을, Emacs는 Ctrl+N/P 및 Ctrl+Opt+B/F를 탐색에 사용하며 검색창에서 Emacs 편집 기능도 제공합니다.</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Default and Vim use Ctrl+J/K and Ctrl+H/L; Emacs uses Ctrl+N/P and Ctrl+Alt+B/F for navigation, plus Emacs editing in the search bar.</source>
        <translation>기본 방식과 Vim은 Ctrl+J/K 및 Ctrl+H/L을 사용합니다. Emacs는 Ctrl+N/P 및 Ctrl+Alt+B/F를 탐색에 사용하며 검색창에서 Emacs 편집 기능도 제공합니다.</translation>
    </message>
    <message>
        <location line="+12"/>
        <source>Search</source>
        <translation>검색</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Root file search</source>
        <translation>메인 화면 파일 검색</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Files are searched asynchronously, so if enabled you should expect a slight delay for file search results to show up.</source>
        <translation>파일은 비동기적으로 검색되므로 이 기능을 켜면 파일 검색 결과가 표시되기까지 약간의 지연이 있을 수 있습니다.</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Favicon Fetching</source>
        <translation>파비콘 가져오기</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>The favicon provider used to load favicons where needed. Select &apos;None&apos; to turn off favicon loading.</source>
        <translation>필요할 때 파비콘을 불러오는 데 사용할 제공자입니다. 파비콘을 불러오지 않으려면 &apos;없음&apos;을 선택하세요.</translation>
    </message>
    <message>
        <location line="+12"/>
        <source>System</source>
        <translation>시스템</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Input server</source>
        <translation>입력 서버</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Whether to spawn the input server at startup. This needs to be enabled in order to support snippets, paste to active window, and other features that require input monitoring or injection.</source>
        <translation>시작 시 입력 서버를 실행할지 설정합니다. 스니펫, 활성 창에 붙여넣기 등 입력 감지나 입력 주입이 필요한 기능을 사용하려면 켜야 합니다.</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Tray icon</source>
        <translation>트레이 아이콘</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Show the Vicinae icon in the system tray. You may need to restart Vicinae.</source>
        <translation>시스템 트레이에 Vicinae 아이콘을 표시합니다. Vicinae를 다시 시작해야 할 수 있습니다.</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Update notice</source>
        <translation>업데이트 알림</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Show a notice in the launcher when an update is available.</source>
        <translation>업데이트가 있으면 런처에 알림을 표시합니다.</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Security</source>
        <translation>보안</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Encrypt sensitive data</source>
        <translation>민감한 데이터 암호화</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Encrypt sensitive data at rest, such as clipboard history and internal databases (OAuth tokens, extension local storage, API keys). Note that some components, such as on-disk clipboard history, may not be retroactively affected when toggling this option. Turning on this option may ask you to unlock your keychain. Requires a restart in order to apply.</source>
        <translation>클립보드 기록과 내부 데이터베이스(OAuth 토큰, 확장 프로그램 로컬 저장소, API 키) 등 저장된 민감한 데이터를 암호화합니다. 이 옵션을 변경해도 디스크에 저장된 클립보드 기록 등 일부 데이터에는 소급 적용되지 않을 수 있습니다. 이 옵션을 켜면 키체인 잠금 해제를 요청할 수 있습니다. 적용하려면 다시 시작해야 합니다.</translation>
    </message>
</context>
<context>
    <name>AiExtension</name>
    <message>
        <location filename="../src/builtins/ai/ai-extension.hpp" line="+39"/>
        <source>AI</source>
        <translation>AI</translation>
    </message>
</context>
<context>
    <name>AiPreferences</name>
    <message>
        <location filename="../src/services/ai/ai-preferences.cpp" line="+39"/>
        <source>None</source>
        <translation>없음</translation>
    </message>
</context>
<context>
    <name>AlertWidget</name>
    <message>
        <location filename="../src/ui/alert/alert.hpp" line="+15"/>
        <source>Are you sure?</source>
        <translation>계속할까요?</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>This action cannot be undone</source>
        <translation>이 작업은 되돌릴 수 없습니다</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Confirm</source>
        <translation>확인</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Cancel</source>
        <translation>취소</translation>
    </message>
</context>
<context>
    <name>AliasFormView</name>
    <message>
        <location filename="../src/ui/qml/views/AliasFormView.qml" line="+16"/>
        <source>Alias</source>
        <translation>별칭</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Additional words to index this item against</source>
        <translation>이 항목을 검색할 때 사용할 추가 단어</translation>
    </message>
</context>
<context>
    <name>AliasFormViewHost</name>
    <message>
        <location filename="../src/builtins/root/alias-form-view-host.cpp" line="+27"/>
        <source>Set alias - %1</source>
        <translation>별칭 설정 - %1</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Submit</source>
        <translation>제출</translation>
    </message>
    <message>
        <location line="+13"/>
        <source>Alias modified</source>
        <translation>별칭을 변경했습니다</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Failed to modify alias</source>
        <translation>별칭을 변경하지 못했습니다</translation>
    </message>
</context>
<context>
    <name>AppRootItem</name>
    <message>
        <location filename="../src/root-search/apps/app-root-provider.cpp" line="+18"/>
        <location line="+24"/>
        <source>Application</source>
        <translation>앱</translation>
    </message>
    <message>
        <location line="-9"/>
        <source>Name</source>
        <translation>이름</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Where</source>
        <translation>위치</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Opens in terminal</source>
        <translation>터미널에서 열기</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Yes</source>
        <translation>예</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>No</source>
        <translation>아니요</translation>
    </message>
    <message>
        <location line="+20"/>
        <source>Open Application</source>
        <translation>앱 열기</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Copy App ID</source>
        <translation>앱 ID 복사</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Copy App Location</source>
        <translation>앱 위치 복사</translation>
    </message>
</context>
<context>
    <name>AppRootProvider</name>
    <message>
        <location line="+94"/>
        <source>Applications</source>
        <translation>앱</translation>
    </message>
</context>
<context>
    <name>AppSelectorModel</name>
    <message>
        <location filename="../src/ui/views/app-selector-model.cpp" line="+18"/>
        <location line="+49"/>
        <source>%1 (Default)</source>
        <translation>%1 (기본값)</translation>
    </message>
</context>
<context>
    <name>AppearanceSettingsPage</name>
    <message>
        <location filename="../src/ui/qml/settings/AppearanceSettingsPage.qml" line="+35"/>
        <location line="+7"/>
        <source>Theme</source>
        <translation>테마</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Font</source>
        <translation>글꼴</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Font size</source>
        <translation>글꼴 크기</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>The base point size used to compute font sizes. Fractional values are accepted. Recommended range is [10.0;12.0].</source>
        <translation>글꼴 크기를 계산하는 기준 포인트 크기입니다. 소수도 입력할 수 있습니다. 권장 범위는 [10.0;12.0]입니다.</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>e.g. 11</source>
        <translation>예: 11</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>Icon Theme</source>
        <translation>아이콘 테마</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>The icon theme used for system icons (applications, mime types, folder icons...). Does not affect builtin Vicinae icons.</source>
        <translation>시스템 아이콘(앱, MIME 유형, 폴더 아이콘 등)에 사용할 아이콘 테마입니다. Vicinae 내장 아이콘에는 영향을 주지 않습니다.</translation>
    </message>
    <message>
        <location line="+12"/>
        <source>Window</source>
        <translation>창</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Window material</source>
        <translation>창 재질</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Background material applied to the launcher window. Lower the window opacity to see it.</source>
        <translation>런처 창에 적용할 배경 재질입니다. 창 불투명도를 낮추면 효과를 볼 수 있습니다.</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Window opacity</source>
        <translation>창 불투명도</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>e.g. 1.0</source>
        <translation>예: 1.0</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Compact mode</source>
        <translation>간결한 모드</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Show only the search bar at root; expand when a query is entered.</source>
        <translation>메인 화면에서는 검색창만 표시하고 검색어를 입력하면 확장합니다.</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Floating status bar</source>
        <translation>떠 있는 상태 표시줄</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Let the status bar float over the content, which stays slightly visible under it. Disable to keep the content strictly above the status bar.</source>
        <translation>상태 표시줄을 콘텐츠 위에 띄워 그 아래로 콘텐츠가 살짝 보이도록 합니다. 끄면 콘텐츠가 상태 표시줄 위쪽에만 표시됩니다.</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Use layer shell</source>
        <translation>레이어 셸 사용</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Anchor the launcher as a Wayland layer surface (wlr-layer-shell) instead of a regular window. May require reopening Vicinae to fully apply.</source>
        <translation>런처를 일반 창 대신 Wayland 레이어 표면(wlr-layer-shell)에 배치합니다. 완전히 적용하려면 Vicinae를 다시 열어야 할 수 있습니다.</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Client-side decorations</source>
        <translation>클라이언트 측 창 장식</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Let Vicinae draw its own rounded borders and shadow instead of relying on the windowing system.</source>
        <translation>창 시스템에 맡기지 않고 Vicinae가 직접 둥근 테두리와 그림자를 그립니다.</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Corner rounding</source>
        <translation>모서리 둥글기</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Radius of the launcher window corners, in pixels.</source>
        <translation>런처 창 모서리의 반경입니다. 단위는 픽셀입니다.</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>e.g. 10</source>
        <translation>예: 10</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>Border width</source>
        <translation>테두리 두께</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Thickness of the launcher window border, in pixels.</source>
        <translation>런처 창 테두리의 두께입니다. 단위는 픽셀입니다.</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>e.g. 3</source>
        <translation>예: 3</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>Shadow size</source>
        <translation>그림자 크기</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Size of the drop shadow cast by the launcher window, in pixels.</source>
        <translation>런처 창에 드리워지는 그림자의 크기입니다. 단위는 픽셀입니다.</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>e.g. 12</source>
        <translation>예: 12</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Native font rendering</source>
        <translation>네이티브 글꼴 렌더링</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Use the platform&apos;s native text rendering for system-consistent text. Disable for Qt distance-field rendering (usually faster). May require reopening Vicinae to fully apply.</source>
        <translation>플랫폼의 기본 텍스트 렌더링을 사용해 시스템과 일관되게 표시합니다. 끄면 Qt 거리 필드 렌더링(일반적으로 더 빠름)을 사용합니다. 완전히 적용하려면 Vicinae를 다시 열어야 할 수 있습니다.</translation>
    </message>
</context>
<context>
    <name>AppleShortcutRootItem</name>
    <message>
        <location filename="../src/root-search/apple-shortcuts/apple-shortcut-root-provider.cpp" line="+66"/>
        <location line="+3"/>
        <source>Apple Shortcut</source>
        <translation>Apple 단축어</translation>
    </message>
</context>
<context>
    <name>AppleShortcutRootProvider</name>
    <message>
        <location line="+30"/>
        <source>Could not load Apple Shortcuts</source>
        <translation>Apple 단축어를 불러올 수 없습니다</translation>
    </message>
    <message>
        <location line="+14"/>
        <source>Apple Shortcuts</source>
        <translation>Apple 단축어</translation>
    </message>
</context>
<context>
    <name>AttachmentCard</name>
    <message>
        <location filename="../src/ui/qml/attachments/AttachmentCard.qml" line="+79"/>
        <source>Preparing…</source>
        <translation>준비 중…</translation>
    </message>
    <message>
        <location line="+17"/>
        <source>Remove %1</source>
        <translation>%1 제거</translation>
    </message>
</context>
<context>
    <name>AttachmentContent</name>
    <message>
        <location filename="../src/ui/quick/attachment-content.cpp" line="+17"/>
        <source>The original file is no longer available.</source>
        <translation>원본 파일을 더 이상 사용할 수 없습니다.</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Could not copy the file.</source>
        <translation>파일을 복사할 수 없습니다.</translation>
    </message>
    <message>
        <location line="+15"/>
        <source>Could not copy the attachment contents.</source>
        <translation>첨부 파일 내용을 복사할 수 없습니다.</translation>
    </message>
</context>
<context>
    <name>AttachmentDropArea</name>
    <message>
        <location filename="../src/ui/qml/attachments/AttachmentDropArea.qml" line="+26"/>
        <source>Drop images or text files</source>
        <translation>이미지나 텍스트 파일을 놓으세요</translation>
    </message>
</context>
<context>
    <name>AttachmentMenu</name>
    <message>
        <location filename="../src/ui/qml/attachments/AttachmentMenu.qml" line="+16"/>
        <source>Preview Image</source>
        <translation>이미지 미리보기</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Copy File</source>
        <translation>파일 복사</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Copy Image</source>
        <translation>이미지 복사</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Copy Text</source>
        <translation>텍스트 복사</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Remove Attachment</source>
        <translation>첨부 파일 제거</translation>
    </message>
</context>
<context>
    <name>AttachmentModel</name>
    <message>
        <location filename="../src/ui/quick/attachment-model.cpp" line="+60"/>
        <source>You can attach up to %1 files per message.</source>
        <translation>메시지당 최대 %1개의 파일을 첨부할 수 있습니다.</translation>
    </message>
    <message>
        <location line="+23"/>
        <source>Text attachments must total less than %1 KB per message.</source>
        <translation>텍스트 첨부 파일의 총 크기는 메시지당 %1 KB 미만이어야 합니다.</translation>
    </message>
    <message>
        <location line="+52"/>
        <source>Pasted image</source>
        <translation>붙여넣은 이미지</translation>
    </message>
    <message>
        <location line="+42"/>
        <source>This image could not be read.</source>
        <translation>이 이미지를 읽을 수 없습니다.</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>This image could not be prepared.</source>
        <translation>이 이미지를 준비할 수 없습니다.</translation>
    </message>
    <message>
        <location line="+13"/>
        <source>Choose an image or a text file.</source>
        <translation>이미지나 텍스트 파일을 선택하세요.</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>This file could not be opened.</source>
        <translation>이 파일을 열 수 없습니다.</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Files must be smaller than %1 MB.</source>
        <translation>파일 크기는 %1 MB 미만이어야 합니다.</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>This file could not be read.</source>
        <translation>이 파일을 읽을 수 없습니다.</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>This file is too large.</source>
        <translation>파일이 너무 큽니다.</translation>
    </message>
    <message>
        <location line="+14"/>
        <location line="+8"/>
        <source>Only images and UTF-8 text files are supported.</source>
        <translation>이미지와 UTF-8 텍스트 파일만 지원합니다.</translation>
    </message>
    <message>
        <location line="-6"/>
        <source>Text files must be smaller than %1 KB.</source>
        <translation>텍스트 파일 크기는 %1 KB 미만이어야 합니다.</translation>
    </message>
</context>
<context>
    <name>AvailableFallbackSection</name>
    <message>
        <location filename="../src/builtins/vicinae/manage-fallback-model.cpp" line="+50"/>
        <source>Enable fallback</source>
        <translation>대체 동작 활성화</translation>
    </message>
    <message>
        <location filename="../src/builtins/vicinae/manage-fallback-model.hpp" line="+49"/>
        <source>Available</source>
        <translation>사용 가능</translation>
    </message>
</context>
<context>
    <name>BashRun</name>
    <message>
        <location filename="../src/builtins/system/bash-tool.cpp" line="+59"/>
        <source>Command timed out.</source>
        <translation>명령 실행 시간이 초과되었습니다.</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Could not start Bash: %1</source>
        <translation>Bash를 시작할 수 없습니다: %1</translation>
    </message>
    <message>
        <location line="+18"/>
        <source>Command cancelled.</source>
        <translation>명령이 취소되었습니다.</translation>
    </message>
    <message>
        <location line="+39"/>
        <source>
[Output truncated at 64 KiB]</source>
        <translation>
[출력이 64 KiB에서 잘렸습니다]</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Command crashed.</source>
        <translation>명령 실행 중 비정상 종료되었습니다.</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Exit code %1</source>
        <translation>종료 코드 %1</translation>
    </message>
</context>
<context>
    <name>BashTool</name>
    <message>
        <location line="+64"/>
        <source>Enter a Bash command without NUL characters.</source>
        <translation>NUL 문자가 없는 Bash 명령을 입력하세요.</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>The timeout must be between 1 and 120000 milliseconds.</source>
        <translation>제한 시간은 1~120000밀리초여야 합니다.</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>The working directory must be an existing absolute path.</source>
        <translation>작업 디렉터리는 실제로 존재하는 절대 경로여야 합니다.</translation>
    </message>
</context>
<context>
    <name>BringToWorkspaceAction</name>
    <message>
        <location filename="../src/actions/window-actions.hpp" line="+72"/>
        <source>Bring to current workspace</source>
        <translation>현재 작업 공간으로 가져오기</translation>
    </message>
</context>
<context>
    <name>BrowseAppsSection</name>
    <message>
        <location filename="../src/builtins/system/browse-apps-model.cpp" line="+19"/>
        <source>Hidden</source>
        <translation>숨김</translation>
    </message>
    <message>
        <location line="+16"/>
        <source>Open Application</source>
        <translation>앱 열기</translation>
    </message>
    <message>
        <location line="+17"/>
        <source>Copy App ID</source>
        <translation>앱 ID 복사</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Copy App Location</source>
        <translation>앱 위치 복사</translation>
    </message>
    <message>
        <location filename="../src/builtins/system/browse-apps-model.hpp" line="+32"/>
        <source>Applications ({count})</source>
        <translation>앱 ({count})</translation>
    </message>
</context>
<context>
    <name>BrowseAppsViewHost</name>
    <message>
        <location filename="../src/builtins/system/browse-apps-view-host.cpp" line="+13"/>
        <source>Search apps...</source>
        <translation>앱 검색...</translation>
    </message>
</context>
<context>
    <name>BrowseFontsCommand</name>
    <message>
        <location filename="../src/builtins/font/browse-fonts-command.hpp" line="+8"/>
        <source>Search Fonts</source>
        <translation>글꼴 검색</translation>
    </message>
</context>
<context>
    <name>BrowserBookmarksSection</name>
    <message>
        <location filename="../src/builtins/browser/browser-bookmarks-model.cpp" line="+38"/>
        <source>Copy URL</source>
        <translation>URL 복사</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Copy Title</source>
        <translation>제목 복사</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Convert to shortcut</source>
        <translation>바로가기 만들기</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Hide Details</source>
        <translation>세부 정보 숨기기</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Show Details</source>
        <translation>세부 정보 표시</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Delete Bookmark</source>
        <translation>북마크 삭제</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Delete bookmark?</source>
        <translation>북마크를 삭제할까요?</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>&quot;%1&quot; will be removed from your browser. This cannot be undone.</source>
        <translation>브라우저에서 &quot;%1&quot;을 삭제합니다. 이 작업은 되돌릴 수 없습니다.</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Failed to delete bookmark: %1</source>
        <translation>북마크를 삭제하지 못했습니다: %1</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Bookmark deleted</source>
        <translation>북마크를 삭제했습니다</translation>
    </message>
    <message>
        <location filename="../src/builtins/browser/browser-bookmarks-model.hpp" line="+21"/>
        <source>Bookmarks ({count})</source>
        <translation>북마크 ({count})</translation>
    </message>
</context>
<context>
    <name>BrowserBookmarksView</name>
    <message>
        <location filename="../src/ui/qml/views/BrowserBookmarksView.qml" line="+27"/>
        <source>No results</source>
        <translation>검색 결과 없음</translation>
    </message>
</context>
<context>
    <name>BrowserBookmarksViewHost</name>
    <message>
        <location filename="../src/builtins/browser/browser-bookmarks-view-host.cpp" line="+59"/>
        <source>Search bookmarks...</source>
        <translation>북마크 검색...</translation>
    </message>
    <message>
        <location line="+75"/>
        <source>All Bookmarks</source>
        <translation>모든 북마크</translation>
    </message>
    <message>
        <location line="+50"/>
        <source>Title</source>
        <translation>제목</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>URL</source>
        <translation>URL</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Folder</source>
        <translation>폴더</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Added</source>
        <translation>추가한 날짜</translation>
    </message>
    <message>
        <location line="+25"/>
        <source>Bookmark access needed</source>
        <translation>북마크 접근 권한 필요</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Browser extension update needed</source>
        <translation>브라우저 확장 프로그램 업데이트 필요</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>No browser connected</source>
        <translation>연결된 브라우저 없음</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Allow the Vicinae browser extension to read your bookmarks. Your browser will ask you to confirm.</source>
        <translation>Vicinae 브라우저 확장 프로그램에 북마크 읽기 권한을 허용하세요. 브라우저에서 확인을 요청합니다.</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Update the Vicinae browser extension to search your bookmarks.</source>
        <translation>북마크를 검색하려면 Vicinae 브라우저 확장 프로그램을 업데이트하세요.</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>You need to connect at least one browser to vicinae using the browser extension in order to use this command.</source>
        <translation>이 명령을 사용하려면 브라우저 확장 프로그램을 통해 하나 이상의 브라우저를 Vicinae에 연결해야 합니다.</translation>
    </message>
    <message>
        <location line="+38"/>
        <source>Allow in %1</source>
        <translation>%1에서 허용</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Failed to request access: %1</source>
        <translation>접근 권한을 요청하지 못했습니다: %1</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Check your browser to allow access</source>
        <translation>브라우저에서 접근 권한을 허용하세요</translation>
    </message>
    <message>
        <location line="+12"/>
        <source>Open documentation</source>
        <translation>문서 열기</translation>
    </message>
</context>
<context>
    <name>BrowserExtension</name>
    <message>
        <location filename="../src/builtins/browser/browser-extension.hpp" line="+12"/>
        <source>Browser Extension</source>
        <translation>브라우저 확장 프로그램</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Browser extension related commands.</source>
        <translation>브라우저 확장 프로그램 관련 명령입니다.</translation>
    </message>
</context>
<context>
    <name>BrowserHistorySection</name>
    <message>
        <location filename="../src/builtins/browser/browser-history-model.cpp" line="+36"/>
        <source>Copy URL</source>
        <translation>URL 복사</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Copy Title</source>
        <translation>제목 복사</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Convert to shortcut</source>
        <translation>바로가기 만들기</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Hide Details</source>
        <translation>세부 정보 숨기기</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Show Details</source>
        <translation>세부 정보 표시</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Delete from History</source>
        <translation>방문 기록에서 삭제</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Delete from history?</source>
        <translation>방문 기록에서 삭제할까요?</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Every visit to %1 will be removed from your browser history. This cannot be undone.</source>
        <translation>브라우저 기록에서 %1의 모든 방문 기록을 삭제합니다. 이 작업은 되돌릴 수 없습니다.</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Failed to delete from history: %1</source>
        <translation>방문 기록에서 삭제하지 못했습니다: %1</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Deleted from history</source>
        <translation>방문 기록에서 삭제했습니다</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Clear Browser History</source>
        <translation>브라우저 방문 기록 지우기</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Browser is no longer connected</source>
        <translation>브라우저 연결이 끊겼습니다</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Clear all %1 history?</source>
        <translation>%1의 모든 기록을 지울까요?</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Your entire browsing history in %1 will be deleted. This cannot be undone.</source>
        <translation>%1의 전체 방문 기록이 삭제됩니다. 이 작업은 되돌릴 수 없습니다.</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Failed to clear history: %1</source>
        <translation>기록을 지우지 못했습니다: %1</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>History cleared</source>
        <translation>기록을 지웠습니다</translation>
    </message>
    <message>
        <location filename="../src/builtins/browser/browser-history-model.hpp" line="+22"/>
        <source>History ({count}+)</source>
        <translation>방문 기록 ({count}+)</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>History ({count})</source>
        <translation>방문 기록 ({count})</translation>
    </message>
</context>
<context>
    <name>BrowserHistoryView</name>
    <message>
        <location filename="../src/ui/qml/views/BrowserHistoryView.qml" line="+27"/>
        <source>No results</source>
        <translation>검색 결과 없음</translation>
    </message>
</context>
<context>
    <name>BrowserHistoryViewHost</name>
    <message>
        <location filename="../src/builtins/browser/browser-history-view-host.cpp" line="+31"/>
        <source>Search history...</source>
        <translation>방문 기록 검색...</translation>
    </message>
    <message>
        <location line="+104"/>
        <source>Title</source>
        <translation>제목</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>URL</source>
        <translation>URL</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Last Visited</source>
        <translation>마지막 방문</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Visits</source>
        <translation>방문 횟수</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>History access needed</source>
        <translation>방문 기록 접근 권한 필요</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Browser extension update needed</source>
        <translation>브라우저 확장 프로그램 업데이트 필요</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>No browser connected</source>
        <translation>연결된 브라우저 없음</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Allow the Vicinae browser extension to read your browsing history. Your browser will ask you to confirm.</source>
        <translation>Vicinae 브라우저 확장 프로그램에 방문 기록 읽기 권한을 허용하세요. 브라우저에서 확인을 요청합니다.</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Update the Vicinae browser extension to search your browsing history.</source>
        <translation>방문 기록을 검색하려면 Vicinae 브라우저 확장 프로그램을 업데이트하세요.</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>You need to connect at least one browser to vicinae using the browser extension in order to use this command.</source>
        <translation>이 명령을 사용하려면 브라우저 확장 프로그램을 통해 하나 이상의 브라우저를 Vicinae에 연결해야 합니다.</translation>
    </message>
    <message>
        <location line="+37"/>
        <source>Allow in %1</source>
        <translation>%1에서 허용</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Failed to request access: %1</source>
        <translation>접근 권한을 요청하지 못했습니다: %1</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Check your browser to allow access</source>
        <translation>브라우저에서 접근 권한을 허용하세요</translation>
    </message>
    <message>
        <location line="+12"/>
        <source>Open documentation</source>
        <translation>문서 열기</translation>
    </message>
</context>
<context>
    <name>BrowserTabActionGenerator</name>
    <message>
        <location filename="../src/actions/browser-tab-actions.hpp" line="+24"/>
        <source>Switch to tab</source>
        <translation>탭으로 전환</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Failed to switch to tab: %1</source>
        <translation>탭으로 전환하지 못했습니다: %1</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Convert to shortcut</source>
        <translation>바로가기 만들기</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Close tab</source>
        <translation>탭 닫기</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Failed to close tab: %1</source>
        <translation>탭을 닫지 못했습니다: %1</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Copy URL</source>
        <translation>URL 복사</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Copy Title</source>
        <translation>제목 복사</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Copy ID</source>
        <translation>ID 복사</translation>
    </message>
</context>
<context>
    <name>BrowserTabProvider</name>
    <message>
        <location filename="../src/root-search/browser-tabs/browser-tabs-provider.hpp" line="+69"/>
        <source>Browser Tabs</source>
        <translation>브라우저 탭</translation>
    </message>
</context>
<context>
    <name>BrowserTabRootItem</name>
    <message>
        <location line="-51"/>
        <source>Browser Tab</source>
        <translation>브라우저 탭</translation>
    </message>
    <message>
        <location line="+14"/>
        <source>Tab</source>
        <translation>탭</translation>
    </message>
</context>
<context>
    <name>BrowserTabsSection</name>
    <message>
        <location filename="../src/builtins/browser/browser-tabs-model.cpp" line="+13"/>
        <source>Muted</source>
        <translation>음소거됨</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Playing</source>
        <translation>재생 중</translation>
    </message>
    <message>
        <location filename="../src/builtins/browser/browser-tabs-model.hpp" line="+22"/>
        <source>Playing Media ({count})</source>
        <translation>재생 중인 미디어 ({count})</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Tabs ({count})</source>
        <translation>탭 ({count})</translation>
    </message>
</context>
<context>
    <name>BrowserTabsViewHost</name>
    <message>
        <location filename="../src/builtins/browser/browser-tabs-view-host.cpp" line="+12"/>
        <source>Search, focus and close tabs</source>
        <translation>탭 검색, 전환 및 닫기</translation>
    </message>
</context>
<context>
    <name>BuiltinIconsSection</name>
    <message>
        <location filename="../src/builtins/vicinae/builtin-icons-model.cpp" line="+15"/>
        <source>Copy Icon Name</source>
        <translation>아이콘 이름 복사</translation>
    </message>
    <message>
        <location filename="../src/builtins/vicinae/builtin-icons-model.hpp" line="+20"/>
        <source>Icons ({count})</source>
        <translation>아이콘 ({count})</translation>
    </message>
</context>
<context>
    <name>BuiltinIconsViewHost</name>
    <message>
        <location filename="../src/builtins/vicinae/builtin-icons-view-host.cpp" line="+10"/>
        <source>Search icons...</source>
        <translation>아이콘 검색...</translation>
    </message>
</context>
<context>
    <name>CalcHistoryListView</name>
    <message>
        <location filename="../src/ui/qml/views/CalcHistoryListView.qml" line="+13"/>
        <source>No results</source>
        <translation>검색 결과 없음</translation>
    </message>
</context>
<context>
    <name>CalcHistorySection</name>
    <message>
        <location filename="../src/builtins/calculator/calc-history-model.cpp" line="+39"/>
        <source>Copy answer</source>
        <translation>답 복사</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Copy question</source>
        <translation>수식 복사</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Copy question and answer</source>
        <translation>수식과 답 복사</translation>
    </message>
</context>
<context>
    <name>CalcHistoryViewHost</name>
    <message>
        <location filename="../src/builtins/calculator/calc-history-view-host.cpp" line="+92"/>
        <source>Search past calculations...</source>
        <translation>이전 계산 검색...</translation>
    </message>
</context>
<context>
    <name>CalcLiveSection</name>
    <message>
        <location line="-14"/>
        <source>Copy unformatted answer</source>
        <translation>서식 없이 답 복사</translation>
    </message>
    <message>
        <location filename="../src/builtins/calculator/calc-history-view-host.hpp" line="+28"/>
        <source>Calculator</source>
        <translation>계산기</translation>
    </message>
</context>
<context>
    <name>CalculatorExtension</name>
    <message>
        <location filename="../src/builtins/calculator/calculator-extension.hpp" line="+95"/>
        <source>Calculator</source>
        <translation>계산기</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Do maths, convert units or search past calculations...</source>
        <translation>계산, 단위 변환 또는 이전 계산 검색...</translation>
    </message>
</context>
<context>
    <name>CalculatorHistoryCommand</name>
    <message>
        <location line="-78"/>
        <source>Calculator history</source>
        <translation>계산 기록</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Browse past calculations. You need to copy the result of a calculation for it to be saved in history.</source>
        <translation>이전 계산을 확인합니다. 계산 결과를 복사해야 기록에 저장됩니다.</translation>
    </message>
</context>
<context>
    <name>CalculatorRefreshRatesCommand</name>
    <message>
        <location line="+11"/>
        <source>Refresh Exchange Rates</source>
        <translation>환율 새로고침</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Refresh exchange rates used by the calculator to provide currency conversion features. Not all backends may support currency conversions or manually refreshing the rates.</source>
        <translation>계산기의 통화 변환 기능에 사용하는 환율을 새로고침합니다. 일부 백엔드는 통화 변환이나 수동 환율 새로고침을 지원하지 않을 수 있습니다.</translation>
    </message>
    <message>
        <location line="+17"/>
        <source>%1 can&apos;t refresh rates</source>
        <translation>%1은 환율을 새로고침할 수 없습니다</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Refreshing rates...</source>
        <translation>환율 새로고침 중...</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Rates successfully refreshed</source>
        <translation>환율을 새로고침했습니다</translation>
    </message>
</context>
<context>
    <name>CalculatorResultDelegate</name>
    <message>
        <location filename="../src/ui/qml/list/CalculatorResultDelegate.qml" line="+61"/>
        <source>Expression</source>
        <translation>수식</translation>
    </message>
    <message>
        <location line="+59"/>
        <source>Result</source>
        <translation>결과</translation>
    </message>
</context>
<context>
    <name>CalculatorService</name>
    <message>
        <location filename="../src/services/calculator-service/calculator-service.cpp" line="+125"/>
        <source>Pinned</source>
        <translation>고정됨</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Today</source>
        <translation>오늘</translation>
    </message>
    <message>
        <location line="+12"/>
        <source>This week</source>
        <translation>이번 주</translation>
    </message>
    <message>
        <location line="+13"/>
        <source>This month</source>
        <translation>이번 달</translation>
    </message>
    <message>
        <location line="+13"/>
        <source>This year</source>
        <translation>올해</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>A few years ago</source>
        <translation>몇 년 전</translation>
    </message>
</context>
<context>
    <name>ChangeEmojiSkinToneAction</name>
    <message>
        <location filename="../src/builtins/vicinae/emoji-grid-model.cpp" line="+102"/>
        <source>%1 skin tone</source>
        <translation>%1 피부색</translation>
    </message>
</context>
<context>
    <name>ChatComposer</name>
    <message>
        <location filename="../src/ui/qml/controls/ChatComposer.qml" line="+202"/>
        <source>Attach images or text files</source>
        <translation>이미지나 텍스트 파일 첨부</translation>
    </message>
    <message>
        <location line="+19"/>
        <source>Choose a model</source>
        <translation>모델 선택</translation>
    </message>
    <message>
        <location line="+21"/>
        <source>Stop response</source>
        <translation>응답 중지</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Send message</source>
        <translation>메시지 보내기</translation>
    </message>
</context>
<context>
    <name>ChatConversation</name>
    <message>
        <location filename="../src/ui/qml/chat/ChatConversation.qml" line="+63"/>
        <source>Find in conversation…</source>
        <translation>대화에서 찾기…</translation>
    </message>
    <message>
        <location line="+14"/>
        <source>Add an AI provider to start a conversation.</source>
        <translation>대화를 시작하려면 AI 제공자를 추가하세요.</translation>
    </message>
    <message>
        <location line="+12"/>
        <source>Set up AI</source>
        <translation>AI 설정</translation>
    </message>
    <message>
        <location line="+44"/>
        <source>Message…</source>
        <translation>메시지…</translation>
    </message>
</context>
<context>
    <name>ChatDocumentBlock</name>
    <message numerus="yes">
        <location filename="../src/ui/qml/views/ChatDocumentBlock.qml" line="+120"/>
        <source>%n tool calls</source>
        <translation>
            <numerusform>도구 호출 %n회</numerusform>
        </translation>
    </message>
    <message>
        <location line="+1"/>
        <source>%1 · %2</source>
        <translation>%1 · %2</translation>
    </message>
    <message>
        <location line="+55"/>
        <source>Thinking</source>
        <translation>생각 중</translation>
    </message>
</context>
<context>
    <name>ChatGreeting</name>
    <message>
        <location filename="../src/ui/qml/views/ChatGreeting.qml" line="+12"/>
        <source>Still up? 🌙</source>
        <translation>아직 깨어 있나요? 🌙</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Hello, night owl 🦉</source>
        <translation>안녕하세요, 올빼미님 🦉</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Good morning ☀️</source>
        <translation>좋은 아침이에요 ☀️</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Morning ☕</source>
        <translation>좋은 아침 ☕</translation>
    </message>
    <message>
        <location line="+0"/>
        <location line="+0"/>
        <location line="+0"/>
        <source>Hello 👋</source>
        <translation>안녕하세요 👋</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Good afternoon 👋</source>
        <translation>좋은 오후예요 👋</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Afternoon!</source>
        <translation>좋은 오후!</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Good evening 👋</source>
        <translation>좋은 저녁이에요 👋</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Evening!</source>
        <translation>좋은 저녁!</translation>
    </message>
    <message>
        <location line="+20"/>
        <source>Choose a model to start chatting.</source>
        <translation>채팅을 시작하려면 모델을 선택하세요.</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Send a message to start a conversation.</source>
        <translation>메시지를 보내 대화를 시작하세요.</translation>
    </message>
</context>
<context>
    <name>ChatSession</name>
    <message>
        <location filename="../src/builtins/ai/chat-session.cpp" line="+70"/>
        <source>Read</source>
        <translation>읽기</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Edit</source>
        <translation>편집</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Delete</source>
        <translation>삭제</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Move</source>
        <translation>이동</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Search</source>
        <translation>검색</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Run Command</source>
        <translation>명령 실행</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Think</source>
        <translation>생각하기</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Fetch</source>
        <translation>가져오기</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Switch Mode</source>
        <translation>모드 전환</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Tool</source>
        <translation>도구</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Load Tools</source>
        <translation>도구 불러오기</translation>
    </message>
    <message>
        <location line="+42"/>
        <source>Could not open conversation.</source>
        <translation>대화를 열 수 없습니다.</translation>
    </message>
    <message>
        <location line="+100"/>
        <source>This conversation was deleted.</source>
        <translation>삭제된 대화입니다.</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Choose a model that supports images for this conversation.</source>
        <translation>이 대화에 사용할 이미지 지원 모델을 선택하세요.</translation>
    </message>
    <message>
        <location line="+330"/>
        <source>This response was interrupted.</source>
        <translation>응답이 중단되었습니다.</translation>
    </message>
    <message>
        <location filename="../src/builtins/ai/chat-session.hpp" line="+78"/>
        <source>New chat</source>
        <translation>새 채팅</translation>
    </message>
</context>
<context>
    <name>ChatSidebar</name>
    <message>
        <location filename="../src/ui/qml/chat/ChatSidebar.qml" line="+78"/>
        <source>Hide sidebar</source>
        <translation>사이드바 숨기기</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>Search conversations</source>
        <translation>대화 검색</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>New chat</source>
        <translation>새 채팅</translation>
    </message>
    <message>
        <location line="+96"/>
        <source>Conversation title</source>
        <translation>대화 제목</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>Rename…</source>
        <translation>이름 변경…</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Delete…</source>
        <translation>삭제…</translation>
    </message>
    <message>
        <location line="+16"/>
        <source>Your conversations will appear here.</source>
        <translation>대화가 여기에 표시됩니다.</translation>
    </message>
    <message>
        <location line="+13"/>
        <source>AI settings</source>
        <translation>AI 설정</translation>
    </message>
</context>
<context>
    <name>ChatTranscript</name>
    <message>
        <location filename="../src/ui/qml/views/ChatTranscript.qml" line="+161"/>
        <source>Jump to latest</source>
        <translation>최신 메시지로 이동</translation>
    </message>
</context>
<context>
    <name>ChatWindow</name>
    <message>
        <location filename="../src/ui/qml/chat/ChatWindow.qml" line="+23"/>
        <source>%1 — Vicinae</source>
        <translation>%1 — Vicinae</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>AI Chat — Vicinae</source>
        <translation>AI Chat — Vicinae</translation>
    </message>
    <message>
        <location line="+107"/>
        <source>Show sidebar</source>
        <translation>사이드바 표시</translation>
    </message>
    <message>
        <location line="+13"/>
        <location line="+19"/>
        <source>New chat</source>
        <translation>새 채팅</translation>
    </message>
    <message>
        <location line="-18"/>
        <source>Conversation title</source>
        <translation>대화 제목</translation>
    </message>
    <message>
        <location line="+30"/>
        <source>Conversation actions</source>
        <translation>대화 동작</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Find in conversation…</source>
        <translation>대화에서 찾기…</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Rename…</source>
        <translation>이름 변경…</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Delete…</source>
        <translation>삭제…</translation>
    </message>
    <message>
        <location line="+53"/>
        <source>Dismiss</source>
        <translation>닫기</translation>
    </message>
    <message>
        <location line="+33"/>
        <source>Delete conversation?</source>
        <translation>대화를 삭제할까요?</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>“%1” and its attachments will be permanently deleted.</source>
        <translation>“%1” 대화와 첨부 파일이 영구적으로 삭제됩니다.</translation>
    </message>
    <message>
        <location line="+12"/>
        <source>Cancel</source>
        <translation>취소</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Delete</source>
        <translation>삭제</translation>
    </message>
    <message>
        <location filename="../src/ui/windows/chat-window.cpp" line="+158"/>
        <source>Could not delete conversation.</source>
        <translation>대화를 삭제할 수 없습니다.</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>Could not open conversation search.</source>
        <translation>대화 검색을 열 수 없습니다.</translation>
    </message>
</context>
<context>
    <name>ClearBrowserHistoryCommand</name>
    <message>
        <location filename="../src/builtins/browser/browser-extension.cpp" line="+128"/>
        <source>Clear Browser History</source>
        <translation>브라우저 방문 기록 지우기</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Clear the browsing history of all connected browsers that allowed history access</source>
        <translation>방문 기록 접근을 허용한 모든 연결된 브라우저의 방문 기록을 지웁니다</translation>
    </message>
    <message>
        <location line="+12"/>
        <source>No browser connected</source>
        <translation>연결된 브라우저 없음</translation>
    </message>
    <message>
        <location line="+14"/>
        <source>No browser allowed history access</source>
        <translation>방문 기록 접근을 허용한 브라우저가 없습니다</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Clear browser history?</source>
        <translation>브라우저 방문 기록을 지울까요?</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Your entire browsing history in %1 will be deleted. This cannot be undone.</source>
        <translation>%1의 전체 방문 기록이 삭제됩니다. 이 작업은 되돌릴 수 없습니다.</translation>
    </message>
    <message>
        <location line="+15"/>
        <source>Failed to clear history: %1</source>
        <translation>기록을 지우지 못했습니다: %1</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>History cleared</source>
        <translation>기록을 지웠습니다</translation>
    </message>
</context>
<context>
    <name>ClearClipboardHistoryCommand</name>
    <message>
        <location filename="../src/builtins/clipboard/clipboard-extension.cpp" line="+38"/>
        <source>Clear Clipboard History</source>
        <translation>클립보드 기록 지우기</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Clear the clipboard history</source>
        <translation>클립보드 기록을 지웁니다</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>Are you sure?</source>
        <translation>계속할까요?</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Your clipboard history will be gone forever :(</source>
        <translation>클립보드 기록이 영구적으로 삭제됩니다 :(</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Failed to clear clipboard history</source>
        <translation>클립보드 기록을 지우지 못했습니다</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Clipboard history cleared</source>
        <translation>클립보드 기록을 지웠습니다</translation>
    </message>
</context>
<context>
    <name>ClipboardClearCommand</name>
    <message>
        <location line="-43"/>
        <source>Clear Current Clipboard Data</source>
        <translation>현재 클립보드 데이터 지우기</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Clear the current content of the clipboard</source>
        <translation>클립보드의 현재 내용을 지웁니다</translation>
    </message>
    <message>
        <location line="+12"/>
        <source>Failed to clear clipboard</source>
        <translation>클립보드를 지우지 못했습니다</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Clipboard cleared</source>
        <translation>클립보드를 지웠습니다</translation>
    </message>
</context>
<context>
    <name>ClipboardExtension</name>
    <message>
        <location line="+37"/>
        <source>Search Clipboard History</source>
        <translation>클립보드 기록 검색</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Find entries in your clipboard history.</source>
        <translation>클립보드 기록에서 항목을 찾습니다.</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Read Clipboard Entry</source>
        <translation>클립보드 항목 읽기</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Read the full text of a clipboard history entry.</source>
        <translation>클립보드 기록 항목의 전체 텍스트를 읽습니다.</translation>
    </message>
    <message>
        <location filename="../src/builtins/clipboard/clipboard-extension.hpp" line="+13"/>
        <source>Clipboard</source>
        <translation>클립보드</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>System clipboard integration</source>
        <translation>시스템 클립보드 연동</translation>
    </message>
</context>
<context>
    <name>ClipboardHistoryCommand</name>
    <message>
        <location filename="../src/builtins/clipboard/clipboard-history-command.hpp" line="+12"/>
        <source>Clipboard History</source>
        <translation>클립보드 기록</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Browse your clipboard&apos;s history, pin, edit and remove entries.</source>
        <translation>클립보드 기록을 살펴보고 항목을 고정, 편집, 삭제합니다.</translation>
    </message>
</context>
<context>
    <name>ClipboardHistorySection</name>
    <message>
        <location filename="../src/builtins/clipboard/history/clipboard-history-model.cpp" line="+43"/>
        <source>Failed to read clipboard data</source>
        <translation>클립보드 데이터를 읽지 못했습니다</translation>
    </message>
    <message>
        <location line="+70"/>
        <source>Open Settings</source>
        <translation>설정 열기</translation>
    </message>
    <message>
        <location line="+84"/>
        <source>Open</source>
        <translation>열기</translation>
    </message>
</context>
<context>
    <name>ClipboardHistoryView</name>
    <message>
        <location filename="../src/ui/qml/views/ClipboardHistoryView.qml" line="+205"/>
        <source>Type</source>
        <translation>유형</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Size</source>
        <translation>크기</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Copied at</source>
        <translation>복사한 시간</translation>
    </message>
    <message>
        <location line="+47"/>
        <source>Preview not available for this content type</source>
        <translation>이 콘텐츠 유형은 미리보기를 지원하지 않습니다</translation>
    </message>
</context>
<context>
    <name>ClipboardHistoryViewHost</name>
    <message>
        <location filename="../src/builtins/clipboard/history/clipboard-history-view-host.cpp" line="+75"/>
        <source>All</source>
        <translation>전체</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Text</source>
        <translation>텍스트</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Images</source>
        <translation>이미지</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Links</source>
        <translation>링크</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Files</source>
        <translation>파일</translation>
    </message>
    <message>
        <location line="+34"/>
        <source>Browse clipboard history...</source>
        <translation>클립보드 기록 검색...</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Clipboard monitoring unavailable</source>
        <translation>클립보드를 감지할 수 없습니다</translation>
    </message>
    <message>
        <location line="+84"/>
        <source>Pause clipboard</source>
        <translation>클립보드 기록 일시 정지</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Resume clipboard</source>
        <translation>클립보드 기록 재개</translation>
    </message>
    <message numerus="yes">
        <location line="+8"/>
        <source>%n Items</source>
        <translation>
            <numerusform>항목 %n개</numerusform>
        </translation>
    </message>
    <message>
        <location line="+30"/>
        <source>Decryption failed</source>
        <translation>암호 해독 실패</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Vicinae could not decrypt the data for this selection. It was most likely encrypted with a different key and cannot be recovered. You can remove this entry from the history.</source>
        <translation>선택한 항목의 데이터를 해독할 수 없습니다. 다른 키로 암호화되었을 가능성이 높으며 복구할 수 없습니다. 기록에서 이 항목을 삭제할 수 있습니다.</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Data unavailable</source>
        <translation>데이터를 사용할 수 없음</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>The data for this selection could not be found on disk.</source>
        <translation>선택한 항목의 데이터를 디스크에서 찾을 수 없습니다.</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Data is encrypted</source>
        <translation>암호화된 데이터</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Data for this selection was previously encrypted but the clipboard is not currently configured to use encryption. You should be able to fix this by enabling it in the settings.</source>
        <translation>선택한 항목의 데이터는 이전에 암호화되었지만 현재 클립보드 설정에서는 암호화를 사용하지 않습니다. 설정에서 암호화를 켜면 해결할 수 있습니다.</translation>
    </message>
    <message>
        <location filename="../src/builtins/clipboard/history/clipboard-history-view-host.hpp" line="+106"/>
        <source>Loading...</source>
        <translation>불러오는 중...</translation>
    </message>
</context>
<context>
    <name>ClipboardService</name>
    <message>
        <location filename="../src/services/clipboard/clipboard-service.cpp" line="+565"/>
        <source>Image (%1x%2)</source>
        <translation>이미지 (%1x%2)</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Image</source>
        <translation>이미지</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Unknown</source>
        <translation>알 수 없음</translation>
    </message>
</context>
<context>
    <name>CloseWindowAction</name>
    <message>
        <location filename="../src/actions/window-actions.hpp" line="-37"/>
        <source>Close window</source>
        <translation>창 닫기</translation>
    </message>
</context>
<context>
    <name>CommandLineSection</name>
    <message>
        <location filename="../src/builtins/system/system-run-model.cpp" line="+38"/>
        <source>Open in %1 (hold)</source>
        <translation>%1에서 열기 (길게 누르기)</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Open in %1</source>
        <translation>%1에서 열기</translation>
    </message>
    <message>
        <location filename="../src/builtins/system/system-run-model.hpp" line="+49"/>
        <source>Execute query</source>
        <translation>쿼리 실행</translation>
    </message>
</context>
<context>
    <name>CommandListView</name>
    <message>
        <location filename="../src/ui/qml/views/CommandListView.qml" line="+13"/>
        <source>No results</source>
        <translation>검색 결과 없음</translation>
    </message>
</context>
<context>
    <name>CommandRootItem</name>
    <message>
        <location filename="../src/root-search/extensions/extension-root-provider.cpp" line="+28"/>
        <location line="+49"/>
        <source>Command</source>
        <translation>명령</translation>
    </message>
    <message>
        <location line="-44"/>
        <location line="+28"/>
        <source>Open command</source>
        <translation>명령 열기</translation>
    </message>
    <message>
        <location line="-13"/>
        <source>Copy extension path</source>
        <translation>확장 프로그램 경로 복사</translation>
    </message>
    <message>
        <location line="+28"/>
        <source>Internal Command</source>
        <translation>내부 명령</translation>
    </message>
</context>
<context>
    <name>CompletionPopup</name>
    <message>
        <location filename="../src/ui/qml/controls/CompletionPopup.qml" line="+15"/>
        <source>Filter...</source>
        <translation>필터링...</translation>
    </message>
</context>
<context>
    <name>ConfigGlobalShortcuts</name>
    <message>
        <location filename="../src/services/global-shortcuts/config-global-shortcuts.cpp" line="+25"/>
        <source>Toggle Vicinae</source>
        <translation>Vicinae 표시 전환</translation>
    </message>
</context>
<context>
    <name>ConversationHistoryCommand</name>
    <message>
        <location filename="../src/builtins/ai/ai-extension.hpp" line="-14"/>
        <source>Browse AI Conversations</source>
        <translation>AI 대화 찾아보기</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Reopen and continue previous AI conversations.</source>
        <translation>이전 AI 대화를 다시 열고 이어서 대화합니다.</translation>
    </message>
</context>
<context>
    <name>ConversationHistoryViewHost</name>
    <message>
        <location filename="../src/builtins/ai/conversation-history-view-host.hpp" line="+30"/>
        <source>Filter conversations...</source>
        <translation>대화 필터링...</translation>
    </message>
    <message>
        <location line="+23"/>
        <source>Open Conversation</source>
        <translation>대화 열기</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>Delete Conversation</source>
        <translation>대화 삭제</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Could not delete conversation.</source>
        <translation>대화를 삭제할 수 없습니다.</translation>
    </message>
    <message>
        <location line="+27"/>
        <source>Could not load conversations.</source>
        <translation>대화를 불러올 수 없습니다.</translation>
    </message>
</context>
<context>
    <name>ConversationListModel</name>
    <message>
        <location filename="../src/builtins/ai/conversation-list-model.cpp" line="+29"/>
        <source>Today</source>
        <translation>오늘</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Yesterday</source>
        <translation>어제</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Previous 7 days</source>
        <translation>지난 7일</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Older</source>
        <translation>이전</translation>
    </message>
    <message>
        <location line="+22"/>
        <source>Could not load conversations.</source>
        <translation>대화를 불러올 수 없습니다.</translation>
    </message>
</context>
<context>
    <name>ConversationStore</name>
    <message>
        <location filename="../src/server.cpp" line="+435"/>
        <source>Could not save conversation.</source>
        <translation>대화를 저장할 수 없습니다.</translation>
    </message>
    <message>
        <location filename="../src/services/ai/conversation-store.cpp" line="+332"/>
        <source>Tool execution was interrupted; no result was recorded.</source>
        <translation>도구 실행이 중단되어 결과가 기록되지 않았습니다.</translation>
    </message>
</context>
<context>
    <name>CopyCalculatorAnswerAction</name>
    <message>
        <location filename="../src/actions/calculator-actions.hpp" line="+29"/>
        <source>Answer copied to clipboard</source>
        <translation>답을 클립보드에 복사했습니다</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Failed to copy answer</source>
        <translation>답을 복사하지 못했습니다</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Copy Result</source>
        <translation>결과 복사</translation>
    </message>
</context>
<context>
    <name>CopyCalculatorQuestionAndAnswerAction</name>
    <message>
        <location line="+18"/>
        <source>Answer copied to clipboard</source>
        <translation>답을 클립보드에 복사했습니다</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Failed to copy answer</source>
        <translation>답을 복사하지 못했습니다</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Copy Question And Answer</source>
        <translation>수식과 답 복사</translation>
    </message>
</context>
<context>
    <name>CopyClipboardSelection</name>
    <message>
        <location filename="../src/builtins/clipboard/history/clipboard-history-actions.hpp" line="+33"/>
        <source>Selection copied to clipboard</source>
        <translation>선택 항목을 클립보드에 복사했습니다</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Failed to copy to clipboard</source>
        <translation>클립보드에 복사하지 못했습니다</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Copy to clipboard</source>
        <translation>클립보드에 복사</translation>
    </message>
</context>
<context>
    <name>CopyItemDeeplink</name>
    <message>
        <location filename="../src/actions/root-search-actions.hpp" line="+99"/>
        <source>Deeplink copied in clipboard</source>
        <translation>딥링크를 클립보드에 복사했습니다</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Copy Deeplink</source>
        <translation>딥링크 복사</translation>
    </message>
</context>
<context>
    <name>CopyShortcutAction</name>
    <message>
        <location filename="../src/actions/shortcut-actions.hpp" line="+269"/>
        <source>Copied to clipboard</source>
        <translation>클립보드에 복사했습니다</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Copy shortcut</source>
        <translation>바로가기 복사</translation>
    </message>
</context>
<context>
    <name>CopyToClipboardAction</name>
    <message>
        <location filename="../src/actions/clipboard-actions.hpp" line="+21"/>
        <source>Copied to clipboard</source>
        <translation>클립보드에 복사했습니다</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Copy to clipboard</source>
        <translation>클립보드에 복사</translation>
    </message>
</context>
<context>
    <name>CreateDictationStyleCommand</name>
    <message>
        <location filename="../src/builtins/dictation/dictation-extension.hpp" line="+46"/>
        <source>Create Dictation Style</source>
        <translation>음성 입력 스타일 만들기</translation>
    </message>
</context>
<context>
    <name>CreateExtensionCommand</name>
    <message>
        <location filename="../src/builtins/developer/developer-extension.hpp" line="+10"/>
        <source>Create Extension</source>
        <translation>확장 프로그램 만들기</translation>
    </message>
</context>
<context>
    <name>CreateExtensionFormView</name>
    <message>
        <location filename="../src/ui/qml/views/CreateExtensionFormView.qml" line="+16"/>
        <source>Author</source>
        <translation>작성자</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>If you plan on submitting your extension to the &lt;a href=&quot;vicinae://launch/core/store&quot;&gt;Vicinae store&lt;/a&gt;, this must exactly match your GitHub handle. Otherwise, you can set it to anything.</source>
        <translation>확장 프로그램을 &lt;a href=&quot;vicinae://launch/core/store&quot;&gt;Vicinae store&lt;/a&gt;에 제출하려면 GitHub 사용자 이름과 정확히 일치해야 합니다. 제출하지 않을 경우 자유롭게 설정할 수 있습니다.</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Username</source>
        <translation>사용자 이름</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Extension Title</source>
        <translation>확장 프로그램 제목</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>My Extension</source>
        <translation>내 확장 프로그램</translation>
    </message>
    <message>
        <location line="+8"/>
        <location line="+43"/>
        <source>Description</source>
        <translation>설명</translation>
    </message>
    <message>
        <location line="-37"/>
        <source>An extension that does super cool things</source>
        <translation>멋진 기능을 제공하는 확장 프로그램</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Location</source>
        <translation>위치</translation>
    </message>
    <message>
        <location line="+16"/>
        <source>Command Title</source>
        <translation>명령 제목</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>My Wonderful Command</source>
        <translation>내 멋진 명령</translation>
    </message>
    <message>
        <location line="+14"/>
        <source>My command does this, and that...</source>
        <translation>이 명령은 이런 일을 합니다...</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Template</source>
        <translation>템플릿</translation>
    </message>
</context>
<context>
    <name>CreateExtensionSuccessViewHost</name>
    <message>
        <location filename="../src/builtins/developer/create-extension-success-view-host.cpp" line="+8"/>
        <source>
# Extension successfully created

Your new extension %1 has been succesfully created at `%2`.

For commands from this extension to be picked up by Vicinae, you need to run your extension in development mode at least once:

```bash
cd &apos;%2&apos;
npm install
npm run dev
```

You can learn more about extension development in the [Vicinae documentation](https://docs.vicinae.com/).
</source>
        <translation>
# 확장 프로그램을 만들었습니다

새 확장 프로그램 %1을 `%2`에 만들었습니다.

Vicinae에서 이 확장 프로그램의 명령을 인식하려면 개발 모드로 한 번 이상 실행해야 합니다:

```bash
cd &apos;%2&apos;
npm install
npm run dev
```

확장 프로그램 개발에 대한 자세한 내용은 [Vicinae 문서](https://docs.vicinae.com/)에서 확인하세요.
</translation>
    </message>
    <message>
        <location line="+37"/>
        <source>Open in %1</source>
        <translation>%1에서 열기</translation>
    </message>
</context>
<context>
    <name>CreateExtensionViewHost</name>
    <message>
        <location filename="../src/builtins/developer/create-extension-view-host.cpp" line="+37"/>
        <source>Create extension</source>
        <translation>확장 프로그램 만들기</translation>
    </message>
    <message>
        <location line="+19"/>
        <location line="+4"/>
        <location line="+19"/>
        <location line="+5"/>
        <source>Min. 3 chars</source>
        <translation>최소 3자</translation>
    </message>
    <message>
        <location line="-20"/>
        <source>Min. 16 chars</source>
        <translation>최소 16자</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Must exist</source>
        <translation>존재해야 합니다</translation>
    </message>
    <message>
        <location line="+18"/>
        <source>Form has errors</source>
        <translation>입력 내용에 오류가 있습니다</translation>
    </message>
    <message>
        <location line="+21"/>
        <source>Failed to create extension</source>
        <translation>확장 프로그램을 만들지 못했습니다</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Extension created!</source>
        <translation>확장 프로그램을 만들었습니다!</translation>
    </message>
</context>
<context>
    <name>CreateShortcutAction</name>
    <message>
        <location filename="../src/actions/shortcut-actions.hpp" line="-53"/>
        <source>Create shortcut</source>
        <translation>바로가기 만들기</translation>
    </message>
</context>
<context>
    <name>CreateShortcutCommand</name>
    <message>
        <location filename="../src/builtins/shortcut/shortcut-extension.hpp" line="+13"/>
        <source>Create Shortcut</source>
        <translation>바로가기 만들기</translation>
    </message>
</context>
<context>
    <name>CreateShortcutFromActiveBrowserTabCommand</name>
    <message>
        <location filename="../src/builtins/browser/browser-extension.cpp" line="-133"/>
        <source>Create Shortcut from Active Tab</source>
        <translation>활성 탭으로 바로가기 만들기</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Create a vicinae shortcut from the currently active browser tab. May yield unexpected results if many browsers are connected at once.</source>
        <translation>현재 활성 브라우저 탭으로 Vicinae 바로가기를 만듭니다. 여러 브라우저가 동시에 연결되어 있으면 예상과 다르게 작동할 수 있습니다.</translation>
    </message>
    <message>
        <location line="+15"/>
        <source>No active tab!</source>
        <translation>활성 탭이 없습니다!</translation>
    </message>
</context>
<context>
    <name>CreateSnippetCommand</name>
    <message>
        <location filename="../src/builtins/snippet/create-snippet-command.hpp" line="+10"/>
        <source>Create Snippet</source>
        <translation>스니펫 만들기</translation>
    </message>
</context>
<context>
    <name>DMenuSection</name>
    <message>
        <location filename="../src/ui/views/dmenu-model.cpp" line="+96"/>
        <source>Select entry</source>
        <translation>항목 선택</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Select entry (index)</source>
        <translation>항목 선택 (인덱스)</translation>
    </message>
    <message>
        <location line="+14"/>
        <source>Pass search text</source>
        <translation>검색어 전달</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Select and copy entry</source>
        <translation>항목 선택 및 복사</translation>
    </message>
</context>
<context>
    <name>DMenuView</name>
    <message>
        <location filename="../src/ui/qml/views/DMenuView.qml" line="+81"/>
        <source>Name</source>
        <translation>이름</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Path</source>
        <translation>경로</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Type</source>
        <translation>유형</translation>
    </message>
</context>
<context>
    <name>DMenuViewHost</name>
    <message>
        <location filename="../src/ui/views/dmenu-view-host.cpp" line="+36"/>
        <source>Search entries...</source>
        <translation>항목 검색...</translation>
    </message>
    <message>
        <location line="+73"/>
        <source>Pass search text</source>
        <translation>검색어 전달</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Pass and copy search text</source>
        <translation>검색어 전달 및 복사</translation>
    </message>
</context>
<context>
    <name>DetailListView</name>
    <message>
        <location filename="../src/ui/qml/views/DetailListView.qml" line="+32"/>
        <source>No results</source>
        <translation>검색 결과 없음</translation>
    </message>
</context>
<context>
    <name>DeveloperExtension</name>
    <message>
        <location filename="../src/builtins/developer/developer-extension.hpp" line="+12"/>
        <source>Developer</source>
        <translation>개발자</translation>
    </message>
</context>
<context>
    <name>DictationExtension</name>
    <message>
        <location filename="../src/builtins/dictation/dictation-extension.hpp" line="+14"/>
        <source>Dictation</source>
        <translation>음성 입력</translation>
    </message>
</context>
<context>
    <name>DictationHistoryCommand</name>
    <message>
        <location line="-49"/>
        <source>Dictation History</source>
        <translation>음성 입력 기록</translation>
    </message>
</context>
<context>
    <name>DictationHistoryViewHost</name>
    <message>
        <location filename="../src/builtins/dictation/dictation-history-view-host.hpp" line="+33"/>
        <source>Search dictations...</source>
        <translation>음성 입력 검색...</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>No dictations yet</source>
        <translation>아직 음성 입력 기록이 없습니다</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Everything you dictate shows up here.</source>
        <translation>음성으로 입력한 내용이 여기에 표시됩니다.</translation>
    </message>
    <message>
        <location line="+28"/>
        <source>Dictated at</source>
        <translation>입력한 시간</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Duration</source>
        <translation>길이</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Words</source>
        <translation>단어 수</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Language</source>
        <translation>언어</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Auto-detected</source>
        <translation>자동 감지</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Style</source>
        <translation>스타일</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Snippet</source>
        <translation>스니펫</translation>
    </message>
    <message>
        <location line="+14"/>
        <source>Copy</source>
        <translation>복사</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Copy Original</source>
        <translation>원문 복사</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Remove Dictation</source>
        <translation>음성 입력 삭제</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Failed to remove dictation</source>
        <translation>음성 입력을 삭제하지 못했습니다</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Remove All Dictations</source>
        <translation>모든 음성 입력 삭제</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Remove all dictations?</source>
        <translation>모든 음성 입력을 삭제할까요?</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Your whole dictation history will be lost forever.</source>
        <translation>전체 음성 입력 기록이 영구적으로 삭제됩니다.</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Remove all</source>
        <translation>모두 삭제</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Dictation history cleared</source>
        <translation>음성 입력 기록을 지웠습니다</translation>
    </message>
</context>
<context>
    <name>DictationRewrite</name>
    <message>
        <location filename="../src/services/dictation/dictation-rewrite.cpp" line="+69"/>
        <source>The model took too long to respond.</source>
        <translation>모델의 응답 시간이 초과되었습니다.</translation>
    </message>
    <message>
        <location line="+18"/>
        <source>The model of this style is not available.</source>
        <translation>이 스타일의 모델을 사용할 수 없습니다.</translation>
    </message>
    <message>
        <location line="+17"/>
        <location line="+12"/>
        <source>Could not start the rewrite.</source>
        <translation>다시 쓰기를 시작할 수 없습니다.</translation>
    </message>
    <message>
        <location line="+17"/>
        <source>The model returned an empty response.</source>
        <translation>모델이 빈 응답을 반환했습니다.</translation>
    </message>
</context>
<context>
    <name>DictationSession</name>
    <message>
        <location filename="../src/services/dictation/dictation-session.cpp" line="+34"/>
        <source>Used original text</source>
        <translation>원문을 사용했습니다</translation>
    </message>
    <message>
        <location line="+13"/>
        <source>Cancel dictation</source>
        <translation>음성 입력 취소</translation>
    </message>
</context>
<context>
    <name>DictationStyleFormView</name>
    <message>
        <location filename="../src/ui/qml/views/DictationStyleFormView.qml" line="+29"/>
        <source>Name</source>
        <translation>이름</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Clean up</source>
        <translation>문장 다듬기</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Instructions</source>
        <translation>지침</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Fix grammar and remove filler words. Keep my wording.</source>
        <translation>내 표현을 유지하면서 문법을 교정하고 불필요한 추임새를 제거하세요.</translation>
    </message>
    <message>
        <location line="+16"/>
        <source>Model</source>
        <translation>모델</translation>
    </message>
    <message>
        <location line="+13"/>
        <source>Applications</source>
        <translation>앱</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Used when you finish dictating in one of these apps.</source>
        <translation>이 앱에서 음성 입력을 마치면 사용합니다.</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>+ Add app…</source>
        <translation>+ 앱 추가…</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Used in apps that have no style of their own.</source>
        <translation>별도 스타일을 지정하지 않은 앱에서 사용합니다.</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Use as default</source>
        <translation>기본값으로 사용</translation>
    </message>
</context>
<context>
    <name>DictationStyleFormViewHost</name>
    <message>
        <location filename="../src/builtins/dictation/style-form-view-host.cpp" line="+33"/>
        <source>Edit &quot;%1&quot;</source>
        <translation>&quot;%1&quot; 편집</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Create Dictation Style</source>
        <translation>음성 입력 스타일 만들기</translation>
    </message>
    <message>
        <location line="+15"/>
        <source>Save Style</source>
        <translation>스타일 저장</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Create Style</source>
        <translation>스타일 만들기</translation>
    </message>
    <message>
        <location line="+52"/>
        <source>%1 already uses %2</source>
        <translation>%1은 이미 %2 스타일을 사용합니다</translation>
    </message>
    <message>
        <location line="+17"/>
        <source>Enter a name</source>
        <translation>이름을 입력하세요</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Another style has this name</source>
        <translation>다른 스타일이 이 이름을 사용하고 있습니다</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Describe how to rewrite the text</source>
        <translation>텍스트를 어떻게 다시 쓸지 설명하세요</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Choose a model</source>
        <translation>모델 선택</translation>
    </message>
    <message>
        <location line="+23"/>
        <source>Failed to save style</source>
        <translation>스타일을 저장하지 못했습니다</translation>
    </message>
    <message>
        <location line="+13"/>
        <source>Style saved</source>
        <translation>스타일을 저장했습니다</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Style created</source>
        <translation>스타일을 만들었습니다</translation>
    </message>
</context>
<context>
    <name>DictationStylesCommand</name>
    <message>
        <location filename="../src/builtins/dictation/dictation-extension.hpp" line="+22"/>
        <source>Manage Dictation Styles</source>
        <translation>음성 입력 스타일 관리</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Have AI rewrite what you dictate, with a different style for each app.</source>
        <translation>앱마다 다른 스타일로 음성 입력 내용을 AI가 다듬도록 설정합니다.</translation>
    </message>
</context>
<context>
    <name>DictationStylesViewHost</name>
    <message>
        <location filename="../src/builtins/dictation/styles-view-host.cpp" line="+28"/>
        <source>Set up an AI provider</source>
        <translation>AI 제공자 설정</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Dictation styles need a language model to rewrite your text. Add an AI provider in the settings.</source>
        <translation>음성 입력 스타일로 텍스트를 다듬으려면 언어 모델이 필요합니다. 설정에서 AI 제공자를 추가하세요.</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Open AI Settings</source>
        <translation>AI 설정 열기</translation>
    </message>
    <message>
        <location line="+43"/>
        <source>Search styles...</source>
        <translation>스타일 검색...</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>No styles yet</source>
        <translation>아직 스타일이 없습니다</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Create a style to have AI rewrite what you dictate.</source>
        <translation>스타일을 만들어 음성 입력 내용을 AI로 다듬으세요.</translation>
    </message>
    <message>
        <location line="+25"/>
        <source>Model unavailable</source>
        <translation>모델을 사용할 수 없음</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>Default</source>
        <translation>기본값</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Not in use</source>
        <translation>사용하지 않음</translation>
    </message>
    <message>
        <location line="+18"/>
        <source>Model</source>
        <translation>모델</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Unavailable</source>
        <translation>사용할 수 없음</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Applications</source>
        <translation>앱</translation>
    </message>
    <message>
        <location line="+14"/>
        <source>Edit Style</source>
        <translation>스타일 편집</translation>
    </message>
    <message>
        <location line="+5"/>
        <location line="+34"/>
        <source>Create Style</source>
        <translation>스타일 만들기</translation>
    </message>
    <message>
        <location line="-28"/>
        <source>Stop Using as Default</source>
        <translation>기본값으로 사용 중지</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Use as Default</source>
        <translation>기본값으로 사용</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Delete Style</source>
        <translation>스타일 삭제</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Delete &quot;%1&quot;?</source>
        <translation>&quot;%1&quot;을 삭제할까요?</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Dictation will no longer use this style.</source>
        <translation>음성 입력에 이 스타일을 더 이상 사용하지 않습니다.</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Delete</source>
        <translation>삭제</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Failed to delete style</source>
        <translation>스타일을 삭제하지 못했습니다</translation>
    </message>
    <message>
        <location filename="../src/builtins/dictation/styles-view-host.hpp" line="+39"/>
        <source>Styles ({count})</source>
        <translation>스타일 ({count})</translation>
    </message>
</context>
<context>
    <name>DisableApplication</name>
    <message>
        <location filename="../src/actions/root-search-actions.hpp" line="+8"/>
        <source>Disable item</source>
        <translation>항목 비활성화</translation>
    </message>
</context>
<context>
    <name>DisableItemAction</name>
    <message>
        <location filename="../src/actions/root-search-actions.cpp" line="+111"/>
        <source>Are you sure?</source>
        <translation>계속할까요?</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>You will need to go in the settings to manually re-enable it.</source>
        <translation>다시 사용하려면 설정에서 직접 활성화해야 합니다.</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Disable</source>
        <translation>비활성화</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Item disabled</source>
        <translation>항목을 비활성화했습니다</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Failed to disable</source>
        <translation>비활성화하지 못했습니다</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Disable item</source>
        <translation>항목 비활성화</translation>
    </message>
</context>
<context>
    <name>DismissNewsAction</name>
    <message>
        <location filename="../src/services/news/news-service.cpp" line="+34"/>
        <source>Dismiss</source>
        <translation>닫기</translation>
    </message>
</context>
<context>
    <name>DocumentFindBar</name>
    <message>
        <location filename="../src/ui/qml/document/DocumentFindBar.qml" line="+10"/>
        <source>Find in document…</source>
        <translation>문서에서 찾기…</translation>
    </message>
    <message>
        <location line="+61"/>
        <source>%1 / %2</source>
        <translation>%1 / %2</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>No matches</source>
        <translation>일치하는 항목 없음</translation>
    </message>
    <message>
        <location line="+13"/>
        <source>Previous match</source>
        <translation>이전 결과</translation>
    </message>
    <message>
        <location line="+12"/>
        <source>Next match</source>
        <translation>다음 결과</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>Close find</source>
        <translation>찾기 닫기</translation>
    </message>
</context>
<context>
    <name>DocumentSelectionMenu</name>
    <message>
        <location filename="../src/ui/qml/document/DocumentSelectionMenu.qml" line="+10"/>
        <source>Copy</source>
        <translation>복사</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Select All</source>
        <translation>모두 선택</translation>
    </message>
</context>
<context>
    <name>DuplicateShortcutAction</name>
    <message>
        <location filename="../src/actions/shortcut-actions.hpp" line="-26"/>
        <source>Duplicate link</source>
        <translation>링크 복제</translation>
    </message>
</context>
<context>
    <name>EditAppleShortcutAction</name>
    <message>
        <location filename="../src/root-search/apple-shortcuts/apple-shortcut-root-provider.cpp" line="-75"/>
        <source>Edit in Shortcuts</source>
        <translation>단축어에서 편집</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Failed to open shortcut</source>
        <translation>단축어를 열지 못했습니다</translation>
    </message>
</context>
<context>
    <name>EditClipboardKeywordsAction</name>
    <message>
        <location filename="../src/builtins/clipboard/history/clipboard-history-actions.hpp" line="+106"/>
        <source>Additional keywords that will be used to index this selection.</source>
        <translation>선택한 항목을 검색할 때 사용할 추가 키워드입니다.</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Edit keywords</source>
        <translation>키워드 편집</translation>
    </message>
</context>
<context>
    <name>EditEmojiKeywordsAction</name>
    <message>
        <location filename="../src/builtins/vicinae/emoji-grid-model.cpp" line="+35"/>
        <source>Additional keywords that will be used to index this glyph</source>
        <translation>이 문자 기호를 검색할 때 사용할 추가 키워드</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Edit keyword</source>
        <translation>키워드 편집</translation>
    </message>
</context>
<context>
    <name>EditKeywordsFormView</name>
    <message>
        <location filename="../src/ui/qml/views/EditKeywordsFormView.qml" line="+20"/>
        <source>Keywords</source>
        <translation>키워드</translation>
    </message>
</context>
<context>
    <name>EditKeywordsViewHost</name>
    <message>
        <location filename="../src/ui/views/edit-keywords-view-host.cpp" line="+26"/>
        <source>Submit</source>
        <translation>제출</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>Keywords edited</source>
        <translation>키워드를 편집했습니다</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Failed to edit keywords</source>
        <translation>키워드를 편집하지 못했습니다</translation>
    </message>
</context>
<context>
    <name>EditShortcutAction</name>
    <message>
        <location filename="../src/actions/shortcut-actions.hpp" line="-41"/>
        <source>Edit shortcut</source>
        <translation>바로가기 편집</translation>
    </message>
</context>
<context>
    <name>EmojiGridModel</name>
    <message>
        <location filename="../src/builtins/vicinae/emoji-grid-model.cpp" line="+262"/>
        <source>Pinned</source>
        <translation>고정됨</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Recently used</source>
        <translation>최근 사용</translation>
    </message>
    <message>
        <location filename="../src/builtins/vicinae/emoji-grid-model.hpp" line="+68"/>
        <source>Search for emojis and symbols...</source>
        <translation>이모지 및 기호 검색...</translation>
    </message>
</context>
<context>
    <name>EmojiGridViewHost</name>
    <message>
        <location filename="../src/builtins/vicinae/emoji-grid-view-host.hpp" line="+60"/>
        <source>All</source>
        <translation>전체</translation>
    </message>
</context>
<context>
    <name>EmptyView</name>
    <message>
        <location filename="../src/ui/qml/views/EmptyView.qml" line="+9"/>
        <source>No results</source>
        <translation>검색 결과 없음</translation>
    </message>
</context>
<context>
    <name>EnabledFallbackSection</name>
    <message>
        <location filename="../src/builtins/vicinae/manage-fallback-model.cpp" line="-22"/>
        <source>Disable fallback</source>
        <translation>대체 동작 비활성화</translation>
    </message>
    <message>
        <location filename="../src/builtins/vicinae/manage-fallback-model.hpp" line="-19"/>
        <source>Enabled</source>
        <translation>활성화됨</translation>
    </message>
</context>
<context>
    <name>Expansion</name>
    <message>
        <location filename="../src/services/snippet/snippet-db.hpp" line="+32"/>
        <source>Keyword cannot be empty</source>
        <translation>키워드는 비워 둘 수 없습니다</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Keyword exceeds maximum length of %1</source>
        <translation>키워드가 최대 길이 %1자를 초과합니다</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Keyword must only contain printable ASCII characters (no spaces)</source>
        <translation>키워드에는 표시 가능한 ASCII 문자만 사용할 수 있습니다 (공백 제외)</translation>
    </message>
</context>
<context>
    <name>ExtensionBoilerplateGenerator</name>
    <message>
        <location filename="../src/services/extension-boilerplate-generator/extension-boilerplate-generator.cpp" line="+32"/>
        <source>Simple List</source>
        <translation>기본 목록</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>List with Detail</source>
        <translation>세부 정보가 있는 목록</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Controlled List</source>
        <translation>제어형 목록</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Simple Detail</source>
        <translation>기본 세부 정보</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>No View</source>
        <translation>화면 없음</translation>
    </message>
</context>
<context>
    <name>ExtensionErrorViewHost</name>
    <message>
        <location filename="../src/extension/views/extension-error-view-host.cpp" line="+7"/>
        <source># Extension crashed 💥!

This extension threw an uncaught exception and crashed as a result.

Find the full stacktrace below. You can also directly copy it from the action menu.

```
%1
```</source>
        <translation># 확장 프로그램이 비정상 종료되었습니다 💥!

이 확장 프로그램에서 처리되지 않은 예외가 발생해 비정상 종료되었습니다.

전체 스택 추적은 아래에 표시됩니다. 동작 메뉴에서 바로 복사할 수도 있습니다.

```
%1
```</translation>
    </message>
</context>
<context>
    <name>ExtensionFormModel</name>
    <message>
        <location filename="../src/extension/views/extension-form-model.cpp" line="+229"/>
        <source>One or more fields have errors</source>
        <translation>하나 이상의 필드에 오류가 있습니다</translation>
    </message>
</context>
<context>
    <name>ExtensionGridModel</name>
    <message>
        <location filename="../src/extension/views/extension-grid-model.cpp" line="+233"/>
        <source>Search...</source>
        <translation>검색...</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>No results</source>
        <translation>검색 결과 없음</translation>
    </message>
</context>
<context>
    <name>ExtensionGridView</name>
    <message>
        <location filename="../src/ui/qml/views/ExtensionGridView.qml" line="+11"/>
        <source>No results</source>
        <translation>검색 결과 없음</translation>
    </message>
</context>
<context>
    <name>ExtensionListModel</name>
    <message>
        <location filename="../src/extension/views/extension-list-model.cpp" line="+210"/>
        <source>Search...</source>
        <translation>검색...</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>No results</source>
        <translation>검색 결과 없음</translation>
    </message>
</context>
<context>
    <name>ExtensionSettingsModel</name>
    <message>
        <location filename="../src/ui/settings/extension-settings-model.cpp" line="+522"/>
        <source>Ask every time</source>
        <translation>매번 묻기</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Always allow</source>
        <translation>항상 허용</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Disabled</source>
        <translation>비활성화됨</translation>
    </message>
</context>
<context>
    <name>ExtensionSettingsPage</name>
    <message>
        <location filename="../src/ui/qml/settings/ExtensionSettingsPage.qml" line="+160"/>
        <source>Description</source>
        <translation>설명</translation>
    </message>
    <message>
        <location line="+23"/>
        <source>Preferences</source>
        <translation>환경설정</translation>
    </message>
    <message>
        <location line="+34"/>
        <source>Commands</source>
        <translation>명령</translation>
    </message>
    <message>
        <location line="+118"/>
        <source>Shortcut</source>
        <translation>단축키</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>Add Alias</source>
        <translation>별칭 추가</translation>
    </message>
    <message>
        <location line="+77"/>
        <source>Tools</source>
        <translation>도구</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>AI tools are turned off in the AI extension settings. Your choices below are kept.</source>
        <translation>AI 확장 프로그램 설정에서 AI 도구가 꺼져 있습니다. 아래에서 선택한 설정은 유지됩니다.</translation>
    </message>
    <message>
        <location line="+76"/>
        <source>Nothing to configure</source>
        <translation>설정할 항목 없음</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Commands and preferences will show up here once available.</source>
        <translation>명령과 환경설정이 있으면 여기에 표시됩니다.</translation>
    </message>
</context>
<context>
    <name>ExtensionView</name>
    <message>
        <location filename="../src/ui/qml/views/ExtensionView.qml" line="+114"/>
        <source>No results</source>
        <translation>검색 결과 없음</translation>
    </message>
</context>
<context>
    <name>FileExtension</name>
    <message>
        <location filename="../src/builtins/file/file-extension.hpp" line="+107"/>
        <source>System files</source>
        <translation>시스템 파일</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Integrate with system files</source>
        <translation>시스템 파일과 연동</translation>
    </message>
    <message>
        <location line="+16"/>
        <source>Search Files</source>
        <translation>파일 검색</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Find files on your computer by name.</source>
        <translation>컴퓨터에서 이름으로 파일을 찾습니다.</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Read File</source>
        <translation>파일 읽기</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Read the content of a text file.</source>
        <translation>텍스트 파일의 내용을 읽습니다.</translation>
    </message>
</context>
<context>
    <name>FilePickerDialog</name>
    <message>
        <location filename="../src/ui/qml/controls/FilePickerDialog.qml" line="+46"/>
        <source>Select files</source>
        <translation>파일 선택</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Select a file</source>
        <translation>파일 선택</translation>
    </message>
    <message>
        <location line="+1"/>
        <location line="+7"/>
        <source>Select</source>
        <translation>선택</translation>
    </message>
    <message>
        <location line="-1"/>
        <source>Select a directory</source>
        <translation>디렉터리 선택</translation>
    </message>
</context>
<context>
    <name>FilePreview</name>
    <message>
        <location filename="../src/ui/qml/detail/FilePreview.qml" line="+37"/>
        <source>Preview not available for this file type</source>
        <translation>이 파일 유형은 미리보기를 지원하지 않습니다</translation>
    </message>
</context>
<context>
    <name>FocusWindowAction</name>
    <message>
        <location filename="../src/actions/window-actions.hpp" line="-17"/>
        <source>Focus window</source>
        <translation>창으로 전환</translation>
    </message>
</context>
<context>
    <name>FontBrowserViewHost</name>
    <message>
        <location filename="../src/builtins/font/font-browser-view-host.hpp" line="+55"/>
        <source>All</source>
        <translation>전체</translation>
    </message>
</context>
<context>
    <name>FontExtension</name>
    <message>
        <location filename="../src/builtins/font/font-extension.hpp" line="+9"/>
        <source>Font</source>
        <translation>글꼴</translation>
    </message>
</context>
<context>
    <name>FontGridModel</name>
    <message>
        <location filename="../src/builtins/font/font-grid-model.cpp" line="+152"/>
        <source>All Fonts (%1)</source>
        <translation>모든 글꼴 (%1)</translation>
    </message>
    <message>
        <location line="+15"/>
        <source>Results (%1)</source>
        <translation>결과 (%1)</translation>
    </message>
    <message>
        <location filename="../src/builtins/font/font-grid-model.hpp" line="+53"/>
        <source>Search fonts...</source>
        <translation>글꼴 검색...</translation>
    </message>
</context>
<context>
    <name>Footer</name>
    <message>
        <location filename="../src/ui/qml/launcher/Footer.qml" line="+62"/>
        <source>Actions</source>
        <translation>동작</translation>
    </message>
</context>
<context>
    <name>ForceQuitAppAction</name>
    <message>
        <location filename="../src/actions/app-actions.cpp" line="+96"/>
        <source>Force Quit Application</source>
        <translation>앱 강제 종료</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Failed to force quit %1</source>
        <translation>%1을 강제 종료하지 못했습니다</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Force quit %1</source>
        <translation>%1 강제 종료</translation>
    </message>
</context>
<context>
    <name>ForgetTelemetryCommand</name>
    <message>
        <location filename="../src/builtins/vicinae/vicinae-extension.cpp" line="+178"/>
        <source>Forget Past Vicinae Telemetry</source>
        <translation>이전 Vicinae 사용 통계 익명화</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Asks the vicinae server to anonymize telemetry data that was sent with your vicinae instance ID attached. The ID is only linked to your vicinae install, which has no direct relationship with your system.</source>
        <translation>Vicinae 인스턴스 ID가 첨부되어 전송된 사용 통계를 익명화하도록 Vicinae 서버에 요청합니다. 이 ID는 Vicinae 설치에만 연결되며 시스템과 직접적인 관련이 없습니다.</translation>
    </message>
    <message>
        <location line="+19"/>
        <source>Processing...</source>
        <translation>처리 중...</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Past telemetry was successfully detached from your vicinae user ID.</source>
        <translation>이전 사용 통계에서 Vicinae 사용자 ID 연결을 해제했습니다.</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Failed to forget past telemetry data</source>
        <translation>이전 사용 통계를 익명화하지 못했습니다</translation>
    </message>
</context>
<context>
    <name>FormAppSelector</name>
    <message>
        <location filename="../src/ui/qml/form/FormAppSelector.qml" line="+15"/>
        <source>+ Restrict to app…</source>
        <translation>+ 앱으로 제한…</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>All applications</source>
        <translation>모든 앱</translation>
    </message>
    <message>
        <location line="+98"/>
        <source>Remove</source>
        <translation>제거</translation>
    </message>
</context>
<context>
    <name>FormFilePicker</name>
    <message>
        <location filename="../src/ui/qml/form/FormFilePicker.qml" line="+26"/>
        <location line="+72"/>
        <source>No directory selected</source>
        <translation>선택한 디렉터리 없음</translation>
    </message>
    <message>
        <location line="-72"/>
        <location line="+72"/>
        <source>No file selected</source>
        <translation>선택한 파일 없음</translation>
    </message>
    <message>
        <location line="+13"/>
        <location line="+124"/>
        <source>Remove</source>
        <translation>제거</translation>
    </message>
    <message>
        <location line="+31"/>
        <source>+ Add folder…</source>
        <translation>+ 폴더 추가…</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>+ Add file…</source>
        <translation>+ 파일 추가…</translation>
    </message>
</context>
<context>
    <name>FormPasswordInput</name>
    <message>
        <location filename="../src/ui/qml/form/FormPasswordInput.qml" line="+98"/>
        <source>Hide password</source>
        <translation>비밀번호 숨기기</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Show password</source>
        <translation>비밀번호 표시</translation>
    </message>
</context>
<context>
    <name>GeneralSettingsModel</name>
    <message>
        <location filename="../src/ui/settings/general-settings-model.cpp" line="+208"/>
        <location line="+12"/>
        <source>Automatic</source>
        <translation>자동</translation>
    </message>
    <message>
        <location line="-10"/>
        <location line="+15"/>
        <source>None</source>
        <translation>없음</translation>
    </message>
    <message>
        <location line="-14"/>
        <location line="+14"/>
        <source>Blurred</source>
        <translation>흐림</translation>
    </message>
    <message>
        <location line="-12"/>
        <location line="+12"/>
        <source>Liquid Glass</source>
        <translation>Liquid Glass</translation>
    </message>
    <message>
        <location line="-11"/>
        <source>Window material</source>
        <translation>창 재질</translation>
    </message>
    <message>
        <location line="+27"/>
        <source>Themes</source>
        <translation>테마</translation>
    </message>
    <message>
        <location line="+17"/>
        <source>Fonts</source>
        <translation>글꼴</translation>
    </message>
    <message>
        <location line="+17"/>
        <source>Icon Themes</source>
        <translation>아이콘 테마</translation>
    </message>
    <message>
        <location line="+15"/>
        <source>Favicon Services</source>
        <translation>파비콘 서비스</translation>
    </message>
    <message>
        <location line="+13"/>
        <location line="+10"/>
        <source>Default</source>
        <translation>기본값</translation>
    </message>
    <message>
        <location line="-7"/>
        <source>Keybinding Schemes</source>
        <translation>키 바인딩 방식</translation>
    </message>
    <message>
        <location line="+48"/>
        <location line="+10"/>
        <source>System default</source>
        <translation>시스템 기본값</translation>
    </message>
    <message>
        <location line="-6"/>
        <source>Languages</source>
        <translation>언어</translation>
    </message>
</context>
<context>
    <name>GeneralSettingsPage</name>
    <message>
        <location filename="../src/ui/qml/settings/GeneralSettingsPage.qml" line="+35"/>
        <source>Behavior</source>
        <translation>동작 방식</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Launcher hotkey</source>
        <translation>런처 단축키</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Global shortcut to toggle the Vicinae launcher.</source>
        <translation>Vicinae 런처를 표시하거나 숨기는 전역 단축키입니다.</translation>
    </message>
    <message>
        <location line="+13"/>
        <source>Close on focus loss</source>
        <translation>포커스를 잃으면 닫기</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Close on Escape</source>
        <translation>Escape를 누르면 닫기</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Pressing Escape closes the launcher instead of navigating one view back.</source>
        <translation>Escape를 누르면 이전 화면으로 돌아가는 대신 런처를 닫습니다.</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Pop to root on close</source>
        <translation>닫을 때 메인 화면으로 이동</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Reset the navigation state when the launcher window is closed.</source>
        <translation>런처 창을 닫으면 탐색 상태를 초기화합니다.</translation>
    </message>
    <message>
        <location line="+10"/>
        <location line="+7"/>
        <source>Language</source>
        <translation>언어</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Requires restarting Vicinae to take effect.</source>
        <translation>적용하려면 Vicinae를 다시 시작해야 합니다.</translation>
    </message>
    <message>
        <location line="+12"/>
        <source>Privacy</source>
        <translation>개인정보 보호</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Basic usage statistics</source>
        <translation>기본 사용 통계</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Send basic system and vicinae installation information on startup to help improve Vicinae.</source>
        <translation>Vicinae 개선을 위해 시작 시 기본 시스템 정보와 Vicinae 설치 정보를 전송합니다.</translation>
    </message>
</context>
<context>
    <name>GenericGridView</name>
    <message>
        <location filename="../src/ui/qml/views/GenericGridView.qml" line="+29"/>
        <source>No results</source>
        <translation>검색 결과 없음</translation>
    </message>
</context>
<context>
    <name>GenericListView</name>
    <message>
        <location filename="../src/ui/qml/views/GenericListView.qml" line="+26"/>
        <source>No results</source>
        <translation>검색 결과 없음</translation>
    </message>
</context>
<context>
    <name>Gnome::Workspace</name>
    <message>
        <location filename="../src/services/window-manager/gnome/gnome-workspace.cpp" line="+18"/>
        <source>Workspace %1</source>
        <translation>작업 공간 %1</translation>
    </message>
</context>
<context>
    <name>HibernateCommand</name>
    <message>
        <location filename="../src/builtins/power-management/power-management-extension.cpp" line="+125"/>
        <source>Hibernate System</source>
        <translation>최대 절전 모드</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Suspend the system to disk. This turns off the system completely and saves its state on disk, to be restored on next boot.</source>
        <translation>시스템 상태를 디스크에 저장한 후 전원을 완전히 끕니다. 다음 부팅 시 저장된 상태를 복원합니다.</translation>
    </message>
    <message>
        <location line="+14"/>
        <source>System can&apos;t hibernate</source>
        <translation>최대 절전 모드를 사용할 수 없습니다</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Failed to hibernate</source>
        <translation>최대 절전 모드로 전환하지 못했습니다</translation>
    </message>
</context>
<context>
    <name>HudWindow</name>
    <message>
        <location filename="../src/ui/qml/hud/HudWindow.qml" line="+27"/>
        <source>Dictation</source>
        <translation>음성 입력</translation>
    </message>
    <message>
        <location line="+142"/>
        <source>Stop and transcribe</source>
        <translation>녹음 중지 및 텍스트 변환</translation>
    </message>
    <message>
        <location line="+76"/>
        <source>Rewriting</source>
        <translation>다시 쓰는 중</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Transcribing</source>
        <translation>텍스트로 변환 중</translation>
    </message>
    <message>
        <location line="+33"/>
        <source>Dismiss</source>
        <translation>닫기</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Cancel dictation</source>
        <translation>음성 입력 취소</translation>
    </message>
</context>
<context>
    <name>IconBrowserCommand</name>
    <message>
        <location filename="../src/builtins/vicinae/vicinae-extension.cpp" line="+26"/>
        <source>Search Builtin Icons</source>
        <translation>내장 아이콘 검색</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Search Vicinae builtin set of icons</source>
        <translation>Vicinae 내장 아이콘 모음을 검색합니다</translation>
    </message>
</context>
<context>
    <name>ImageViewer</name>
    <message>
        <location filename="../src/ui/qml/detail/ImageViewer.qml" line="+163"/>
        <source>%1 / %2</source>
        <translation>%1 / %2</translation>
    </message>
</context>
<context>
    <name>InspectLocalStorage</name>
    <message>
        <location filename="../src/builtins/vicinae/vicinae-extension.cpp" line="+13"/>
        <source>Inspect Local Storage</source>
        <translation>로컬 저장소 살펴보기</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Browse data stored in Vicinae&apos;s local storage. This includes data stored for builtin extensions as well as third-party extensions making use of the LocalStorage API.</source>
        <translation>Vicinae 로컬 저장소에 저장된 데이터를 살펴봅니다. 내장 확장 프로그램과 LocalStorage API를 사용하는 타사 확장 프로그램의 데이터가 포함됩니다.</translation>
    </message>
</context>
<context>
    <name>InstallExtensionAction</name>
    <message>
        <location filename="../src/actions/extension-actions.cpp" line="+15"/>
        <source>Downloading extension...</source>
        <translation>확장 프로그램 다운로드 중...</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Failed to download extension</source>
        <translation>확장 프로그램을 다운로드하지 못했습니다</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Failed to extract extension archive</source>
        <translation>확장 프로그램 압축 파일을 풀지 못했습니다</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Extension installed</source>
        <translation>확장 프로그램을 설치했습니다</translation>
    </message>
    <message>
        <location filename="../src/actions/extension-actions.hpp" line="+28"/>
        <source>Install extension</source>
        <translation>확장 프로그램 설치</translation>
    </message>
</context>
<context>
    <name>InstallUpdateAction</name>
    <message>
        <location filename="../src/services/update/update-service.cpp" line="+190"/>
        <source>Install Update</source>
        <translation>업데이트 설치</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>An update is already in progress</source>
        <translation>이미 업데이트가 진행 중입니다</translation>
    </message>
</context>
<context>
    <name>InstalledExtensionsSection</name>
    <message>
        <location filename="../src/builtins/vicinae/installed-extensions-model.cpp" line="+38"/>
        <source>Local</source>
        <translation>로컬</translation>
    </message>
    <message>
        <location line="+16"/>
        <source>Copy</source>
        <translation>복사</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Copy Name</source>
        <translation>이름 복사</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Copy ID</source>
        <translation>ID 복사</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Copy Path</source>
        <translation>경로 복사</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Copy Author</source>
        <translation>작성자 복사</translation>
    </message>
    <message>
        <location filename="../src/builtins/vicinae/installed-extensions-model.hpp" line="+18"/>
        <source>Installed Extensions ({count})</source>
        <translation>설치된 확장 프로그램 ({count})</translation>
    </message>
</context>
<context>
    <name>InstalledExtensionsViewHost</name>
    <message>
        <location filename="../src/builtins/vicinae/installed-extensions-view-host.cpp" line="+12"/>
        <source>Search extensions...</source>
        <translation>확장 프로그램 검색...</translation>
    </message>
</context>
<context>
    <name>InternalExtension</name>
    <message>
        <location filename="../src/builtins/internal/internal-extension.hpp" line="+12"/>
        <location line="+1"/>
        <source>Internal Commands</source>
        <translation>내부 명령</translation>
    </message>
</context>
<context>
    <name>KdeSettingsRootItem</name>
    <message>
        <location filename="../src/root-search/kde-settings/kde-settings-root-provider.cpp" line="+13"/>
        <location line="+9"/>
        <source>KDE Settings</source>
        <translation>KDE 설정</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Name</source>
        <translation>이름</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Where</source>
        <translation>위치</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Open in System Settings</source>
        <translation>시스템 설정에서 열기</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Copy Module ID</source>
        <translation>모듈 ID 복사</translation>
    </message>
</context>
<context>
    <name>KdeSettingsRootProvider</name>
    <message>
        <location line="+17"/>
        <source>KDE Settings</source>
        <translation>KDE 설정</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Modules of the KDE System Settings application.</source>
        <translation>KDE 시스템 설정 앱의 모듈입니다.</translation>
    </message>
</context>
<context>
    <name>KeyboardBridge</name>
    <message>
        <location filename="../src/ui/bridges/keyboard-bridge.hpp" line="+60"/>
        <source>Modifier required</source>
        <translation>보조 키 필요</translation>
    </message>
</context>
<context>
    <name>LauncherStatusBarMacOS</name>
    <message>
        <location filename="../src/ui/qml/launcher/LauncherStatusBarMacOS.qml" line="+23"/>
        <location line="+5"/>
        <source>Vicinae menu</source>
        <translation>Vicinae 메뉴</translation>
    </message>
    <message>
        <location line="+65"/>
        <source>Vicinae actions</source>
        <translation>Vicinae 동작</translation>
    </message>
    <message>
        <location line="+28"/>
        <source>Actions</source>
        <translation>동작</translation>
    </message>
</context>
<context>
    <name>LauncherWindow</name>
    <message>
        <location filename="../src/ui/qml/launcher/LauncherWindow.qml" line="+51"/>
        <source>Vicinae Launcher</source>
        <translation>Vicinae 런처</translation>
    </message>
    <message>
        <location filename="../src/ui/windows/launcher-window.cpp" line="+931"/>
        <source>Open Settings</source>
        <translation>설정 열기</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Keyboard Shortcuts</source>
        <translation>키보드 단축키</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Extension Store</source>
        <translation>Extension Store</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Documentation</source>
        <translation>문서</translation>
    </message>
    <message>
        <location line="+3"/>
        <location line="+5"/>
        <source>Opened in browser</source>
        <translation>브라우저에서 열었습니다</translation>
    </message>
    <message>
        <location line="-2"/>
        <source>Report a Bug</source>
        <translation>버그 신고</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>About Vicinae</source>
        <translation>Vicinae 정보</translation>
    </message>
</context>
<context>
    <name>LocalStorageItemSection</name>
    <message>
        <location filename="../src/builtins/vicinae/local-storage-model.cpp" line="+31"/>
        <source>Show value</source>
        <translation>값 표시</translation>
    </message>
    <message>
        <location filename="../src/builtins/vicinae/local-storage-model.hpp" line="+19"/>
        <source>Items ({count})</source>
        <translation>항목 ({count})</translation>
    </message>
</context>
<context>
    <name>LocalStorageItemViewHost</name>
    <message>
        <location filename="../src/builtins/vicinae/local-storage-view-host.cpp" line="+26"/>
        <source>Search items...</source>
        <translation>항목 검색...</translation>
    </message>
</context>
<context>
    <name>LocalStorageNamespaceSection</name>
    <message>
        <location filename="../src/builtins/vicinae/local-storage-model.cpp" line="-16"/>
        <source>Browse namespace</source>
        <translation>네임스페이스 살펴보기</translation>
    </message>
    <message>
        <location filename="../src/builtins/vicinae/local-storage-model.hpp" line="-11"/>
        <source>Namespaces ({count})</source>
        <translation>네임스페이스 ({count})</translation>
    </message>
</context>
<context>
    <name>LocalStorageViewHost</name>
    <message>
        <location filename="../src/builtins/vicinae/local-storage-view-host.cpp" line="-18"/>
        <source>Search namespaces...</source>
        <translation>네임스페이스 검색...</translation>
    </message>
</context>
<context>
    <name>LockCommand</name>
    <message>
        <location filename="../src/builtins/power-management/power-management-extension.cpp" line="-48"/>
        <source>Lock Session</source>
        <translation>세션 잠금</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Lock the current user session</source>
        <translation>현재 사용자 세션을 잠급니다</translation>
    </message>
    <message>
        <location line="+13"/>
        <source>System can&apos;t lock</source>
        <translation>시스템을 잠글 수 없습니다</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Failed to lock</source>
        <translation>잠그지 못했습니다</translation>
    </message>
</context>
<context>
    <name>LogOutCommand</name>
    <message>
        <location line="+176"/>
        <source>Log Out</source>
        <translation>로그아웃</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Terminate the current user session. If you simply want to lock your session you should use &apos;Lock Session&apos; instead.</source>
        <translation>현재 사용자 세션을 종료합니다. 세션을 잠그기만 하려면 &apos;세션 잠금&apos;을 사용하세요.</translation>
    </message>
    <message>
        <location line="+15"/>
        <source>System can&apos;t logout</source>
        <translation>시스템에서 로그아웃할 수 없습니다</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Failed to log out</source>
        <translation>로그아웃하지 못했습니다</translation>
    </message>
</context>
<context>
    <name>LongTextPreferenceDialog</name>
    <message>
        <location filename="../src/ui/qml/settings/LongTextPreferenceDialog.qml" line="+59"/>
        <source>%1 characters. Long text is sent with every message and costs more.</source>
        <translation>%1자. 긴 텍스트는 메시지마다 전송되므로 비용이 더 듭니다.</translation>
    </message>
    <message numerus="yes">
        <location line="+0"/>
        <source>%n character(s)</source>
        <translation>
            <numerusform>%n자</numerusform>
        </translation>
    </message>
    <message>
        <location line="+17"/>
        <source>Cancel</source>
        <translation>취소</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Save</source>
        <translation>저장</translation>
    </message>
</context>
<context>
    <name>MacOSGlobalShortcutBackend</name>
    <message>
        <location filename="../src/services/global-shortcuts/macos-global-shortcut-backend.cpp" line="+166"/>
        <location line="+44"/>
        <source>unsupported or invalid trigger</source>
        <translation>지원하지 않거나 잘못된 트리거</translation>
    </message>
    <message>
        <location line="-36"/>
        <source>failed to register hot key (%1)</source>
        <translation>단축키를 등록하지 못했습니다 (%1)</translation>
    </message>
</context>
<context>
    <name>ManageFallbackActions</name>
    <message>
        <location filename="../src/actions/fallback-actions.hpp" line="+15"/>
        <source>Manage Fallback Actions</source>
        <translation>대체 동작 관리</translation>
    </message>
</context>
<context>
    <name>ManageFallbackCommand</name>
    <message>
        <location filename="../src/builtins/vicinae/configure-fallback-command.hpp" line="+11"/>
        <source>Configure Fallback Commands</source>
        <translation>대체 명령 설정</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Configure what commands are to be presented as fallback options when nothing matches the search in the root search.</source>
        <translation>메인 화면 검색 결과가 없을 때 대체 동작으로 표시할 명령을 설정합니다.</translation>
    </message>
</context>
<context>
    <name>ManageFallbackViewHost</name>
    <message>
        <location filename="../src/builtins/vicinae/manage-fallback-view-host.cpp" line="+12"/>
        <source>Search commands...</source>
        <translation>명령 검색...</translation>
    </message>
</context>
<context>
    <name>ManageShortcutsCommand</name>
    <message>
        <location filename="../src/builtins/shortcut/shortcut-extension.hpp" line="+13"/>
        <source>Manage Shortcuts</source>
        <translation>바로가기 관리</translation>
    </message>
</context>
<context>
    <name>ManageShortcutsSection</name>
    <message>
        <location filename="../src/builtins/shortcut/manage-shortcuts-model.hpp" line="+17"/>
        <source>Shortcuts ({count})</source>
        <translation>바로가기 ({count})</translation>
    </message>
</context>
<context>
    <name>ManageShortcutsViewHost</name>
    <message>
        <location filename="../src/builtins/shortcut/manage-shortcuts-view-host.cpp" line="+26"/>
        <source>Search shortcuts...</source>
        <translation>바로가기 검색...</translation>
    </message>
    <message>
        <location line="+28"/>
        <source>Name</source>
        <translation>이름</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Application</source>
        <translation>앱</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>%1 (Default)</source>
        <translation>%1 (기본값)</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Opened</source>
        <translation>열어 본 횟수</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Last Opened</source>
        <translation>마지막으로 연 시간</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Never</source>
        <translation>없음</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Created at</source>
        <translation>만든 날짜</translation>
    </message>
</context>
<context>
    <name>ManageSnippetsCommand</name>
    <message>
        <location filename="../src/builtins/snippet/manage-snippets-command.hpp" line="+10"/>
        <source>Manage Snippets</source>
        <translation>스니펫 관리</translation>
    </message>
</context>
<context>
    <name>ManageSnippetsSection</name>
    <message>
        <location filename="../src/builtins/snippet/manage-snippets-model.cpp" line="+33"/>
        <source>Copy to clipboard</source>
        <translation>클립보드에 복사</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Copied to clipboard</source>
        <translation>클립보드에 복사했습니다</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Failed to copy to clipboard</source>
        <translation>클립보드에 복사하지 못했습니다</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Edit snippet</source>
        <translation>스니펫 편집</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Duplicate snippet</source>
        <translation>스니펫 복제</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Remove snippet</source>
        <translation>스니펫 삭제</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Failed to remove snippet</source>
        <translation>스니펫을 삭제하지 못했습니다</translation>
    </message>
    <message>
        <location filename="../src/builtins/snippet/manage-snippets-model.hpp" line="+18"/>
        <source>Snippets ({count})</source>
        <translation>스니펫 ({count})</translation>
    </message>
</context>
<context>
    <name>ManageSnippetsViewHost</name>
    <message>
        <location filename="../src/builtins/snippet/manage-snippets-view-host.cpp" line="+12"/>
        <source>No snippets</source>
        <translation>스니펫 없음</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Create a snippet to get started</source>
        <translation>스니펫을 만들어 시작하세요</translation>
    </message>
    <message>
        <location line="+19"/>
        <source>Search for snippets...</source>
        <translation>스니펫 검색...</translation>
    </message>
    <message>
        <location line="+25"/>
        <source>Text</source>
        <translation>텍스트</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>File</source>
        <translation>파일</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Type</source>
        <translation>유형</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Created at</source>
        <translation>만든 날짜</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Updated at</source>
        <translation>수정한 날짜</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Keyword</source>
        <translation>키워드</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Voice phrase</source>
        <translation>음성 문구</translation>
    </message>
    <message>
        <location line="+17"/>
        <source>Apps</source>
        <translation>앱</translation>
    </message>
    <message>
        <location line="+68"/>
        <source>Create snippet</source>
        <translation>스니펫 만들기</translation>
    </message>
</context>
<context>
    <name>MarkItemAsFavorite</name>
    <message>
        <location filename="../src/actions/root-search-actions.cpp" line="-80"/>
        <source>Mark as favorite</source>
        <translation>즐겨찾기에 추가</translation>
    </message>
</context>
<context>
    <name>MarkdownShowcase</name>
    <message>
        <location filename="../src/builtins/internal/markdown-showcase-command.hpp" line="+193"/>
        <source>Markdown Showcase</source>
        <translation>Markdown 예제</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Preview all supported markdown features</source>
        <translation>지원하는 모든 Markdown 기능 미리보기</translation>
    </message>
</context>
<context>
    <name>MarkdownShowcaseView</name>
    <message>
        <location line="-182"/>
        <source>Close</source>
        <translation>닫기</translation>
    </message>
</context>
<context>
    <name>McpServer</name>
    <message>
        <location filename="../src/services/ai/mcp/mcp-server.cpp" line="+223"/>
        <source>The tool did not return a result.</source>
        <translation>도구가 결과를 반환하지 않았습니다.</translation>
    </message>
</context>
<context>
    <name>MdCallout</name>
    <message>
        <location filename="../src/ui/qml/markdown/MdCallout.qml" line="+35"/>
        <source>Caution</source>
        <translation>주의</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Warning</source>
        <translation>경고</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Important</source>
        <translation>중요</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Tip</source>
        <translation>팁</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Note</source>
        <translation>참고</translation>
    </message>
</context>
<context>
    <name>MdCodeBlock</name>
    <message>
        <location filename="../src/ui/qml/markdown/MdCodeBlock.qml" line="+60"/>
        <source>Copied!</source>
        <translation>복사했습니다!</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Copy</source>
        <translation>복사</translation>
    </message>
</context>
<context>
    <name>MdImage</name>
    <message>
        <location filename="../src/ui/qml/markdown/MdImage.qml" line="+75"/>
        <source>Image</source>
        <translation>이미지</translation>
    </message>
</context>
<context>
    <name>MediaExtension</name>
    <message>
        <location filename="../src/builtins/media/media-extension.hpp" line="+324"/>
        <source>Media</source>
        <translation>미디어</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Control media playback and system audio</source>
        <translation>미디어 재생과 시스템 오디오 제어</translation>
    </message>
</context>
<context>
    <name>MenuBarMenuSection</name>
    <message>
        <location filename="../src/builtins/vicinae/menu-bar-search-view-host.hpp" line="+38"/>
        <source>Results ({count} items)</source>
        <translation>결과 ({count}개 항목)</translation>
    </message>
    <message>
        <location line="+28"/>
        <source>Run Menu Item</source>
        <translation>메뉴 항목 실행</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Open in Menu Bar</source>
        <translation>메뉴 막대에서 열기</translation>
    </message>
</context>
<context>
    <name>MenuBarSearchViewHost</name>
    <message>
        <location line="+22"/>
        <source>Filter by menu item title...</source>
        <translation>메뉴 항목 제목으로 필터링...</translation>
    </message>
</context>
<context>
    <name>MissingPreferenceView</name>
    <message>
        <location filename="../src/ui/qml/views/MissingPreferenceView.qml" line="+30"/>
        <source>Welcome to %1</source>
        <translation>%1에 오신 것을 환영합니다</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Before you can use this command, you need to fill in the required preference fields below.</source>
        <translation>이 명령을 사용하려면 아래 필수 환경설정 항목을 입력해야 합니다.</translation>
    </message>
    <message>
        <location line="+130"/>
        <source>Select an app…</source>
        <translation>앱 선택…</translation>
    </message>
    <message>
        <location line="+19"/>
        <source>Add app…</source>
        <translation>앱 추가…</translation>
    </message>
</context>
<context>
    <name>MissingPreferenceViewHost</name>
    <message>
        <location filename="../src/extension/views/missing-preference-view-host.cpp" line="+238"/>
        <source>Save preferences</source>
        <translation>환경설정 저장</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Please fill in all required fields</source>
        <translation>필수 항목을 모두 입력하세요</translation>
    </message>
</context>
<context>
    <name>ModelDownload</name>
    <message>
        <location filename="../src/services/local-model-registry/model-download.cpp" line="+66"/>
        <source>Could not create %1: %2</source>
        <translation>%1을 만들 수 없습니다: %2</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Could not open %1 for writing: %2</source>
        <translation>%1을 쓰기 모드로 열 수 없습니다: %2</translation>
    </message>
    <message>
        <location line="+99"/>
        <source>Could not write %1: %2</source>
        <translation>%1에 쓸 수 없습니다: %2</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Server could not resume the download</source>
        <translation>서버에서 다운로드를 재개할 수 없습니다</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Rate limited by the server</source>
        <translation>서버의 요청 횟수 제한에 도달했습니다</translation>
    </message>
    <message numerus="yes">
        <location line="+17"/>
        <source>Gave up after %n attempt(s): %1</source>
        <translation>
            <numerusform>%n회 시도 후 중단했습니다: %1</numerusform>
        </translation>
    </message>
    <message>
        <location line="+23"/>
        <source>Downloaded file has the wrong size (%1 instead of %2 bytes)</source>
        <translation>다운로드한 파일의 크기가 잘못되었습니다 (예상: %2바이트, 실제: %1바이트)</translation>
    </message>
    <message>
        <location line="+17"/>
        <source>Checksum mismatch, the downloaded file was discarded</source>
        <translation>체크섬이 일치하지 않아 다운로드한 파일을 삭제했습니다</translation>
    </message>
    <message>
        <location line="+13"/>
        <source>Could not move the download into place: %1</source>
        <translation>다운로드한 파일을 지정된 위치로 옮길 수 없습니다: %1</translation>
    </message>
</context>
<context>
    <name>MoveFavoriteDownAction</name>
    <message>
        <location filename="../src/actions/root-search-actions.cpp" line="+53"/>
        <source>Move down in favorites</source>
        <translation>즐겨찾기에서 아래로 이동</translation>
    </message>
</context>
<context>
    <name>MoveFavoriteUpAction</name>
    <message>
        <location line="-9"/>
        <source>Move up in favorites</source>
        <translation>즐겨찾기에서 위로 이동</translation>
    </message>
</context>
<context>
    <name>NavigationController</name>
    <message>
        <location filename="../src/navigation-controller.cpp" line="+727"/>
        <source>Extension manager is not running</source>
        <translation>확장 프로그램 관리자가 실행 중이 아닙니다</translation>
    </message>
</context>
<context>
    <name>NewsService</name>
    <message>
        <location filename="../src/services/news/news-service.cpp" line="+68"/>
        <source>Telemetry</source>
        <translation>사용 통계</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>We now collect basic usage statistics on startup</source>
        <translation>이제 시작 시 기본 사용 통계를 수집합니다</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Learn more</source>
        <translation>자세히 알아보기</translation>
    </message>
</context>
<context>
    <name>NextTrackCommand</name>
    <message>
        <location filename="../src/builtins/media/media-extension.hpp" line="-217"/>
        <location line="+26"/>
        <source>Next Track</source>
        <translation>다음 트랙</translation>
    </message>
    <message>
        <location line="-25"/>
        <source>Skip to the next track</source>
        <translation>다음 트랙으로 건너뛰기</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>player</source>
        <translation>플레이어</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>%1 cannot skip to the next track</source>
        <translation>%1에서 다음 트랙으로 건너뛸 수 없습니다</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Failed to skip to the next track</source>
        <translation>다음 트랙으로 건너뛰지 못했습니다</translation>
    </message>
</context>
<context>
    <name>NowPlayingCommand</name>
    <message>
        <location line="+46"/>
        <source>Now Playing</source>
        <translation>지금 재생 중</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Browse and control running media players</source>
        <translation>실행 중인 미디어 플레이어를 확인하고 제어합니다</translation>
    </message>
</context>
<context>
    <name>NowPlayingViewHost</name>
    <message>
        <location filename="../src/builtins/media/now-playing-view-host.hpp" line="+27"/>
        <source>Search players...</source>
        <translation>플레이어 검색...</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Players</source>
        <translation>플레이어</translation>
    </message>
    <message>
        <location line="+16"/>
        <source>Playing</source>
        <translation>재생 중</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Paused</source>
        <translation>일시 정지됨</translation>
    </message>
    <message>
        <location line="+12"/>
        <source>Pause</source>
        <translation>일시 정지</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Play</source>
        <translation>재생</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Next Track</source>
        <translation>다음 트랙</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Previous Track</source>
        <translation>이전 트랙</translation>
    </message>
</context>
<context>
    <name>NullUpdateInstaller</name>
    <message>
        <location filename="../src/services/update/null-update-installer.hpp" line="+14"/>
        <source>Self update is not supported on this platform</source>
        <translation>이 플랫폼에서는 자체 업데이트를 지원하지 않습니다</translation>
    </message>
</context>
<context>
    <name>OAuthOverlayView</name>
    <message>
        <location filename="../src/ui/qml/views/OAuthOverlayView.qml" line="+29"/>
        <source>Back</source>
        <translation>뒤로</translation>
    </message>
    <message>
        <location line="+65"/>
        <source>Continue with %1</source>
        <translation>%1으로 계속</translation>
    </message>
    <message>
        <location line="+33"/>
        <source>You&apos;re in!</source>
        <translation>연결되었습니다!</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Successfully connected to %1.
Back to command in an instant...</source>
        <translation>%1에 연결했습니다.
곧 명령 화면으로 돌아갑니다...</translation>
    </message>
</context>
<context>
    <name>OAuthTokenStoreCommand</name>
    <message>
        <location filename="../src/builtins/vicinae/vicinae-extension.cpp" line="-33"/>
        <source>Manage OAuth Token Sets</source>
        <translation>OAuth 토큰 모음 관리</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Manage OAuth token sets that have been saved by extensions providing OAuth integrations.</source>
        <translation>OAuth 연동을 제공하는 확장 프로그램이 저장한 OAuth 토큰 모음을 관리합니다.</translation>
    </message>
</context>
<context>
    <name>OAuthTokenStoreSection</name>
    <message>
        <location filename="../src/builtins/vicinae/oauth-token-store-model.cpp" line="+21"/>
        <source>Expired</source>
        <translation>만료됨</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>Remove token set</source>
        <translation>토큰 모음 삭제</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Are you sure?</source>
        <translation>계속할까요?</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>You will need to go through the OAuth login flow again the next time you want to use this service</source>
        <translation>다음에 이 서비스를 사용하려면 OAuth 로그인 절차를 다시 진행해야 합니다</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Failed to remove token set</source>
        <translation>토큰 모음을 삭제하지 못했습니다</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Token set removed</source>
        <translation>토큰 모음을 삭제했습니다</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Copy</source>
        <translation>복사</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Copy Access Token</source>
        <translation>액세스 토큰 복사</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Copy Refresh Token</source>
        <translation>갱신 토큰 복사</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Copy ID Token</source>
        <translation>ID 토큰 복사</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Copy Scopes</source>
        <translation>권한 범위 복사</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Copy Expiration Date</source>
        <translation>만료일 복사</translation>
    </message>
    <message>
        <location filename="../src/builtins/vicinae/oauth-token-store-model.hpp" line="+17"/>
        <source>OAuth Token Sets ({count})</source>
        <translation>OAuth 토큰 모음 ({count})</translation>
    </message>
</context>
<context>
    <name>OAuthTokenStoreViewHost</name>
    <message>
        <location filename="../src/builtins/vicinae/oauth-token-store-view-host.cpp" line="+12"/>
        <source>Search token sets...</source>
        <translation>토큰 모음 검색...</translation>
    </message>
</context>
<context>
    <name>OnboardingWindow</name>
    <message>
        <location filename="../src/ui/qml/onboarding/OnboardingWindow.qml" line="+41"/>
        <source>Grant Access</source>
        <translation>접근 허용</translation>
    </message>
    <message>
        <location line="+18"/>
        <source>Granted</source>
        <translation>허용됨</translation>
    </message>
    <message>
        <location line="+16"/>
        <location line="+40"/>
        <source>Welcome to Vicinae</source>
        <translation>Vicinae에 오신 것을 환영합니다</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Let&apos;s set it up. It only takes a minute.</source>
        <translation>설정을 시작하세요. 1분이면 충분합니다.</translation>
    </message>
    <message>
        <location line="+20"/>
        <source>Permissions</source>
        <translation>권한</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Vicinae needs additional permissions in order to make the best of your Mac.</source>
        <translation>Mac의 기능을 최대한 활용하려면 Vicinae에 추가 권한이 필요합니다.</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>Accessibility</source>
        <translation>손쉬운 사용</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Used to paste, expand snippets, and move windows.</source>
        <translation>붙여넣기, 스니펫 확장, 창 이동에 사용합니다.</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Full Disk Access</source>
        <translation>전체 디스크 접근 권한</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Allows file search to cover your entire disk.</source>
        <translation>전체 디스크에서 파일을 검색할 수 있도록 허용합니다.</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Notifications</source>
        <translation>알림</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Allows extensions to send desktop notifications.</source>
        <translation>확장 프로그램이 데스크톱 알림을 보낼 수 있도록 허용합니다.</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>Without accessibility access, paste, snippet expansion, and window management are unavailable.</source>
        <translation>손쉬운 사용 권한이 없으면 붙여넣기, 스니펫 확장, 창 관리를 사용할 수 없습니다.</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Full disk access needs to be explicitly enabled if you want file search to cover all your files.</source>
        <translation>모든 파일을 검색하려면 전체 디스크 접근 권한을 직접 허용해야 합니다.</translation>
    </message>
    <message>
        <location line="+19"/>
        <source>Make it your own</source>
        <translation>나에게 맞게 설정하기</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>You will be able to change these settings later.</source>
        <translation>나중에 이 설정을 변경할 수 있습니다.</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>Theme</source>
        <translation>테마</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Shared across the entire app.</source>
        <translation>앱 전체에 적용됩니다.</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>Global hotkey</source>
        <translation>전역 단축키</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Opens the launcher from anywhere.</source>
        <translation>어디서나 런처를 엽니다.</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Bind a key to &quot;vicinae toggle&quot;</source>
        <translation>&quot;vicinae toggle&quot;에 단축키를 지정하세요</translation>
    </message>
    <message>
        <location line="+18"/>
        <source>Open Docs</source>
        <translation>문서 열기</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Launch at login</source>
        <translation>로그인 시 실행</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Starts Vicinae in the background at login.</source>
        <translation>로그인할 때 Vicinae를 백그라운드에서 시작합니다.</translation>
    </message>
    <message>
        <location line="+19"/>
        <source>Setup complete</source>
        <translation>설정 완료</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Vicinae is running. Open the launcher with:</source>
        <translation>Vicinae가 실행 중입니다. 다음 단축키로 런처를 여세요:</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Vicinae is running. Bind a key to &quot;vicinae toggle&quot; to open it from anywhere.</source>
        <translation>Vicinae가 실행 중입니다. 어디서나 열 수 있도록 &quot;vicinae toggle&quot;에 단축키를 지정하세요.</translation>
    </message>
    <message>
        <location line="+17"/>
        <source>Vicinae is open source software.</source>
        <translation>Vicinae는 오픈 소스 소프트웨어입니다.</translation>
    </message>
    <message>
        <location line="+20"/>
        <source>Sponsor</source>
        <translation>후원</translation>
    </message>
    <message>
        <location line="+17"/>
        <source>Back</source>
        <translation>뒤로</translation>
    </message>
    <message>
        <location line="+39"/>
        <source>Finish</source>
        <translation>완료</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Continue</source>
        <translation>계속</translation>
    </message>
</context>
<context>
    <name>OpenAppAction</name>
    <message>
        <location filename="../src/actions/app-actions.cpp" line="-56"/>
        <location filename="../src/builtins/clipboard/history/clipboard-history-actions.hpp" line="-89"/>
        <source>Failed to start app</source>
        <translation>앱을 실행하지 못했습니다</translation>
    </message>
</context>
<context>
    <name>OpenAppLocationAction</name>
    <message>
        <location line="-36"/>
        <source>Open Location</source>
        <translation>위치 열기</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Failed to open app location</source>
        <translation>앱 위치를 열지 못했습니다</translation>
    </message>
</context>
<context>
    <name>OpenBuiltinCommandAction</name>
    <message>
        <location filename="../src/actions/command-actions.hpp" line="+17"/>
        <source>Open command</source>
        <translation>명령 열기</translation>
    </message>
</context>
<context>
    <name>OpenCalculatorHistoryAction</name>
    <message>
        <location filename="../src/actions/calculator-actions.hpp" line="+9"/>
        <source>Open Calculator History</source>
        <translation>계산 기록 열기</translation>
    </message>
</context>
<context>
    <name>OpenClipboardSelectionAction</name>
    <message>
        <location filename="../src/builtins/clipboard/history/clipboard-history-actions.hpp" line="-5"/>
        <source>Failed to read clipboard data</source>
        <translation>클립보드 데이터를 읽지 못했습니다</translation>
    </message>
</context>
<context>
    <name>OpenCompletedShortcutAction</name>
    <message>
        <location filename="../src/actions/shortcut-actions.hpp" line="-32"/>
        <source>Open shortcut</source>
        <translation>바로가기 열기</translation>
    </message>
</context>
<context>
    <name>OpenCompletedShortcutWithAction</name>
    <message>
        <location line="+114"/>
        <source>Open with...</source>
        <translation>다른 앱으로 열기...</translation>
    </message>
</context>
<context>
    <name>OpenControlPanelItemAction</name>
    <message>
        <location filename="../src/root-search/control-panel/control-panel-root-provider.cpp" line="+45"/>
        <source>Failed to open settings</source>
        <translation>설정을 열지 못했습니다</translation>
    </message>
</context>
<context>
    <name>OpenControlPanelTaskAction</name>
    <message>
        <location line="+26"/>
        <source>Failed to open settings</source>
        <translation>설정을 열지 못했습니다</translation>
    </message>
</context>
<context>
    <name>OpenDefaultVicinaeConfig</name>
    <message>
        <location filename="../src/builtins/vicinae/vicinae-extension.cpp" line="-117"/>
        <source>Open Default Config File</source>
        <translation>기본 설정 파일 열기</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Open the default vicinae configuration file</source>
        <translation>Vicinae 기본 설정 파일을 엽니다</translation>
    </message>
    <message>
        <location line="+16"/>
        <source>Failed to open temporary file</source>
        <translation>임시 파일을 열지 못했습니다</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Failed to open default config file</source>
        <translation>기본 설정 파일을 열지 못했습니다</translation>
    </message>
</context>
<context>
    <name>OpenDiscordCommand</name>
    <message>
        <location line="-85"/>
        <source>Join the Discord Server</source>
        <translation>Discord 서버 참여</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Open link to join the official Vicinae discord server.</source>
        <translation>공식 Vicinae Discord 서버 참여 링크를 엽니다.</translation>
    </message>
</context>
<context>
    <name>OpenFileAction</name>
    <message>
        <location filename="../src/actions/file-actions.hpp" line="+18"/>
        <source>Open with %1</source>
        <translation>%1으로 열기</translation>
    </message>
</context>
<context>
    <name>OpenInBrowserAction</name>
    <message>
        <location filename="../src/actions/app-actions.cpp" line="+160"/>
        <source>Failed to open in browser</source>
        <translation>브라우저에서 열지 못했습니다</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Opened in browser</source>
        <translation>브라우저에서 열었습니다</translation>
    </message>
    <message>
        <location filename="../src/actions/app-actions.hpp" line="+117"/>
        <source>Open in browser</source>
        <translation>브라우저에서 열기</translation>
    </message>
</context>
<context>
    <name>OpenInTerminalAction</name>
    <message>
        <location filename="../src/actions/app-actions.cpp" line="-150"/>
        <source>Failed to start app</source>
        <translation>앱을 실행하지 못했습니다</translation>
    </message>
    <message>
        <location filename="../src/actions/app-actions.hpp" line="-74"/>
        <source>Open in %1</source>
        <translation>%1에서 열기</translation>
    </message>
</context>
<context>
    <name>OpenItemPreferencesAction</name>
    <message>
        <location filename="../src/actions/root-search-actions.hpp" line="-28"/>
        <source>Open Preferences</source>
        <translation>환경설정 열기</translation>
    </message>
</context>
<context>
    <name>OpenLastChatCommand</name>
    <message>
        <location filename="../src/builtins/ai/ai-extension.hpp" line="-16"/>
        <source>Open Last AI Chat</source>
        <translation>최근 AI 채팅 열기</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Pick up your most recent AI conversation.</source>
        <translation>가장 최근 AI 대화를 이어서 진행합니다.</translation>
    </message>
</context>
<context>
    <name>OpenRawProgramAction</name>
    <message>
        <location filename="../src/actions/app-actions.cpp" line="+35"/>
        <source>Failed to start app</source>
        <translation>앱을 실행하지 못했습니다</translation>
    </message>
    <message>
        <location filename="../src/actions/app-actions.hpp" line="+25"/>
        <source>Execute program</source>
        <translation>프로그램 실행</translation>
    </message>
</context>
<context>
    <name>OpenSettingsCommand</name>
    <message>
        <location filename="../src/builtins/vicinae/vicinae-extension.cpp" line="+100"/>
        <source>Open Vicinae Settings</source>
        <translation>Vicinae 설정 열기</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Open the vicinae settings window, which is an independent floating window.</source>
        <translation>독립된 창으로 표시되는 Vicinae 설정을 엽니다.</translation>
    </message>
</context>
<context>
    <name>OpenShortcutAction</name>
    <message>
        <location filename="../src/actions/shortcut-actions.hpp" line="-161"/>
        <source>No default app to open %1</source>
        <translation>%1을 열 기본 앱이 없습니다</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>No app with id %1</source>
        <translation>ID가 %1인 앱이 없습니다</translation>
    </message>
    <message>
        <location line="+12"/>
        <location line="+7"/>
        <source>Open shortcut</source>
        <translation>바로가기 열기</translation>
    </message>
</context>
<context>
    <name>OpenShortcutFromSearchText</name>
    <message>
        <location line="+43"/>
        <source>Open shortcut</source>
        <translation>바로가기 열기</translation>
    </message>
</context>
<context>
    <name>OpenVicinaeConfig</name>
    <message>
        <location filename="../src/builtins/vicinae/vicinae-extension.cpp" line="-76"/>
        <source>Open Config File</source>
        <translation>설정 파일 열기</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Open the main vicinae configuration file</source>
        <translation>Vicinae 기본 사용자 설정 파일을 엽니다</translation>
    </message>
    <message>
        <location line="+17"/>
        <source>Show Log File</source>
        <translation>로그 파일 표시</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Open the Vicinae log file in your file browser</source>
        <translation>파일 관리자에서 Vicinae 로그 파일을 엽니다</translation>
    </message>
</context>
<context>
    <name>OpenWindowsSettingAction</name>
    <message>
        <location filename="../src/root-search/windows-settings/windows-settings-root-provider.cpp" line="+147"/>
        <source>Failed to open settings</source>
        <translation>설정을 열지 못했습니다</translation>
    </message>
</context>
<context>
    <name>OpenWithAction</name>
    <message>
        <location filename="../src/actions/app-actions.cpp" line="+119"/>
        <location filename="../src/builtins/clipboard/history/clipboard-history-actions.hpp" line="+36"/>
        <source>Open with...</source>
        <translation>다른 앱으로 열기...</translation>
    </message>
</context>
<context>
    <name>PasteLastScreenshotCommand</name>
    <message>
        <location filename="../src/builtins/screenshots/screenshots-extension.hpp" line="+30"/>
        <source>Paste Last Screenshot</source>
        <translation>최근 스크린샷 붙여넣기</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Paste the most recent saved screenshot into the active app.</source>
        <translation>가장 최근에 저장한 스크린샷을 활성 앱에 붙여넣습니다.</translation>
    </message>
</context>
<context>
    <name>PasteToFocusedWindowAction</name>
    <message>
        <location filename="../src/actions/clipboard-actions.hpp" line="+16"/>
        <source>Paste to %1</source>
        <translation>%1에 붙여넣기</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Paste to active window</source>
        <translation>활성 창에 붙여넣기</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Copy to focused window</source>
        <translation>포커스된 창에 복사</translation>
    </message>
</context>
<context>
    <name>PinCalculatorHistoryRecordAction</name>
    <message>
        <location filename="../src/actions/calculator-actions.hpp" line="+28"/>
        <source>Entry pinned</source>
        <translation>항목을 고정했습니다</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Pin entry</source>
        <translation>항목 고정</translation>
    </message>
</context>
<context>
    <name>PinClipboardAction</name>
    <message>
        <location filename="../src/builtins/clipboard/history/clipboard-history-actions.hpp" line="+31"/>
        <source>Selection pinned</source>
        <translation>선택 항목을 고정했습니다</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Selection unpinned</source>
        <translation>선택 항목의 고정을 해제했습니다</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Failed to change pin status</source>
        <translation>고정 상태를 변경하지 못했습니다</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Pin</source>
        <translation>고정</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Unpin</source>
        <translation>고정 해제</translation>
    </message>
</context>
<context>
    <name>PinEmojiAction</name>
    <message>
        <location filename="../src/builtins/vicinae/emoji-grid-model.cpp" line="-345"/>
        <source>Pin emoji</source>
        <translation>이모지 고정</translation>
    </message>
</context>
<context>
    <name>PinWindowAction</name>
    <message>
        <location filename="../src/actions/window-actions.hpp" line="+37"/>
        <source>Unpin from all workspaces</source>
        <translation>모든 작업 공간에서 고정 해제</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Pin to all workspaces</source>
        <translation>모든 작업 공간에 고정</translation>
    </message>
</context>
<context>
    <name>PlayPauseCommand</name>
    <message>
        <location filename="../src/builtins/media/media-extension.hpp" line="-100"/>
        <source>Play / Pause</source>
        <translation>재생 / 일시 정지</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Toggle playback of the active media player</source>
        <translation>활성 미디어 플레이어의 재생 상태를 전환합니다</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>player</source>
        <translation>플레이어</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Failed to toggle playback</source>
        <translation>재생 상태를 전환하지 못했습니다</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Paused</source>
        <translation>일시 정지됨</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Playing %1</source>
        <translation>%1 재생 중</translation>
    </message>
</context>
<context>
    <name>PowerManagementCommand</name>
    <message>
        <location filename="../src/builtins/power-management/power-management-extension.cpp" line="-248"/>
        <source>Failed to execute custom program %1</source>
        <translation>사용자 지정 프로그램 %1을 실행하지 못했습니다</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Are you sure</source>
        <translation>계속할까요</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>High-impact operation, please confirm</source>
        <translation>시스템에 큰 영향을 주는 작업입니다. 확인해 주세요</translation>
    </message>
</context>
<context>
    <name>PowerManagementExtension</name>
    <message>
        <location filename="../src/builtins/power-management/power-management-extension.hpp" line="+8"/>
        <source>Power Management</source>
        <translation>전원 관리</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Power off, suspend, sleep, hibernate your computer.</source>
        <translation>컴퓨터를 끄거나 대기, 절전, 최대 절전 모드로 전환합니다.</translation>
    </message>
</context>
<context>
    <name>PowerOffCommand</name>
    <message>
        <location filename="../src/builtins/power-management/power-management-extension.cpp" line="+137"/>
        <source>Power Off System</source>
        <translation>시스템 종료</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Power off the system</source>
        <translation>시스템 전원을 끕니다</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>System cannot power off</source>
        <translation>시스템을 종료할 수 없습니다</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Failed to power off</source>
        <translation>시스템을 종료하지 못했습니다</translation>
    </message>
</context>
<context>
    <name>PreferenceSchema</name>
    <message>
        <location filename="../src/builtins/calculator/calculator-extension.hpp" line="+13"/>
        <source>Calculator Backend</source>
        <translation>계산기 백엔드</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Which backend to use to perform calculations</source>
        <translation>계산에 사용할 백엔드</translation>
    </message>
    <message>
        <location filename="../src/builtins/system/system-run-model.hpp" line="-19"/>
        <source>The default action to run on pressing return</source>
        <translation>Return을 눌렀을 때 실행할 기본 동작</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Run in terminal</source>
        <translation>터미널에서 실행</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Run in terminal (hold)</source>
        <translation>터미널에서 실행 (길게 누르기)</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Run directly</source>
        <translation>바로 실행</translation>
    </message>
    <message>
        <location filename="../src/builtins/file/file-extension.hpp" line="-69"/>
        <source>Enabled</source>
        <translation>활성화됨</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Whether to run the file indexer in the background. When turned off, the indexer process is stopped entirely and file search becomes unavailable until it is turned back on.</source>
        <translation>파일 인덱서를 백그라운드에서 실행할지 설정합니다. 끄면 인덱서 프로세스가 완전히 중지되며 다시 켤 때까지 파일 검색을 사용할 수 없습니다.</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Search paths</source>
        <translation>검색 경로</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Directories that Vicinae will search</source>
        <translation>Vicinae에서 검색할 디렉터리</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Excluded search paths</source>
        <translation>제외할 검색 경로</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Directories to exclude from file indexing</source>
        <translation>파일 인덱싱에서 제외할 디렉터리</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Search backend</source>
        <translation>검색 백엔드</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Automatic uses Everything when it is running and falls back to Windows Search otherwise.</source>
        <translation>자동 설정에서는 Everything이 실행 중이면 사용하고, 그렇지 않으면 Windows Search를 사용합니다.</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Automatic</source>
        <translation>자동</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Windows Search</source>
        <translation>Windows Search</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Everything</source>
        <translation>Everything</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Everything instance</source>
        <translation>Everything 인스턴스</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Name of the Everything instance to connect to. Leave empty for the default instance, the Everything 1.5 alpha runs as &quot;1.5a&quot;.</source>
        <translation>연결할 Everything 인스턴스의 이름입니다. 기본 인스턴스를 사용하려면 비워 두세요. Everything 1.5 알파는 &quot;1.5a&quot;로 실행됩니다.</translation>
    </message>
    <message>
        <location filename="../src/builtins/power-management/power-management-extension.cpp" line="-200"/>
        <source>Ask for confirmation</source>
        <translation>확인 요청</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Custom program</source>
        <translation>사용자 지정 프로그램</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Custom shell command to run instead of the default implementation</source>
        <translation>기본 동작 대신 실행할 사용자 지정 셸 명령</translation>
    </message>
    <message>
        <location filename="../src/builtins/clipboard/clipboard-preferences.hpp" line="+64"/>
        <source>Clipboard monitoring</source>
        <translation>클립보드 감지</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Whether new clipboard selections are appended to the history</source>
        <translation>새로 복사한 내용을 클립보드 기록에 추가할지 설정합니다</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Ignore Passwords</source>
        <translation>비밀번호 제외</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Ignore selections that can be identified as a password. May not work with all apps.</source>
        <translation>비밀번호로 식별할 수 있는 내용은 기록하지 않습니다. 일부 앱에서는 작동하지 않을 수 있습니다.</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Preserve tagged</source>
        <translation>태그된 항목 유지</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Never evict or mass delete selections that have been explicitly tagged (pinned, custom keyword)</source>
        <translation>명시적으로 태그를 지정한 항목(고정, 사용자 지정 키워드)은 자동 삭제하거나 일괄 삭제하지 않습니다</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Excluded apps</source>
        <translation>제외할 앱</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Never add selections copied from these apps to the history</source>
        <translation>이 앱에서 복사한 내용은 기록에 추가하지 않습니다</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Eviction threshold</source>
        <translation>보관 기간</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Automatically delete selections older than this threshold</source>
        <translation>이 기간이 지난 항목을 자동으로 삭제합니다</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Never</source>
        <translation>없음</translation>
    </message>
    <message>
        <location line="+1"/>
        <location filename="../src/services/ai/ai-provider-types.hpp" line="+269"/>
        <source>15 minutes</source>
        <translation>15분</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>1 hour</source>
        <translation>1시간</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>1 day</source>
        <translation>1일</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>1 week</source>
        <translation>1주</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>1 month</source>
        <translation>1개월</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>1 year</source>
        <translation>1년</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Erase on startup</source>
        <translation>시작 시 지우기</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Erase clipboard history every time the vicinae server is started</source>
        <translation>Vicinae 서버를 시작할 때마다 클립보드 기록을 지웁니다</translation>
    </message>
    <message>
        <location line="+12"/>
        <location filename="../src/builtins/system/system-run-model.hpp" line="-7"/>
        <location filename="../src/builtins/vicinae/emoji-preferences.hpp" line="+29"/>
        <source>Default Action</source>
        <translation>기본 동작</translation>
    </message>
    <message>
        <location line="+1"/>
        <location filename="../src/builtins/vicinae/emoji-preferences.hpp" line="+1"/>
        <source>The default action to perform on pressing return. Paste is only available if your environment supports it.</source>
        <translation>Return을 눌렀을 때 수행할 기본 동작입니다. 붙여넣기는 사용 환경에서 지원하는 경우에만 사용할 수 있습니다.</translation>
    </message>
    <message>
        <location line="+7"/>
        <location filename="../src/builtins/vicinae/emoji-preferences.hpp" line="+6"/>
        <source>Paste</source>
        <translation>붙여넣기</translation>
    </message>
    <message>
        <location line="+2"/>
        <location filename="../src/builtins/vicinae/emoji-preferences.hpp" line="+2"/>
        <source>Copy</source>
        <translation>복사</translation>
    </message>
    <message>
        <location filename="../src/builtins/dictation/dictation-preferences.hpp" line="+26"/>
        <source>Transcription model</source>
        <translation>음성 인식 모델</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Model used to turn your voice into text. You can configure cloud options, or Vicinae can download and run local transcription models for you.</source>
        <translation>음성을 텍스트로 변환할 모델입니다. 클라우드 서비스를 설정하거나 Vicinae에서 로컬 음성 인식 모델을 다운로드해 실행할 수 있습니다.</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>None</source>
        <translation>없음</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Language</source>
        <translation>언어</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Language you dictate in. Some models can auto-detect it, others need it set explicitly.</source>
        <translation>음성 입력에 사용할 언어입니다. 자동 감지가 가능한 모델도 있고 직접 지정해야 하는 모델도 있습니다.</translation>
    </message>
    <message>
        <location line="+12"/>
        <source>Auto-detect</source>
        <translation>자동 감지</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>All languages</source>
        <translation>모든 언어</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Sound Effects</source>
        <translation>효과음</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Whether to play a sound effect when starting or stopping dictation.</source>
        <translation>음성 입력을 시작하거나 중지할 때 효과음을 재생할지 설정합니다.</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Pause media</source>
        <translation>미디어 일시 정지</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Pause media players when recording and resume them after. The audio output is muted instead when players cannot be controlled.</source>
        <translation>녹음하는 동안 미디어 플레이어를 일시 정지하고 녹음 후 다시 재생합니다. 플레이어를 제어할 수 없으면 대신 오디오 출력을 음소거합니다.</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Dictation Action</source>
        <translation>음성 입력 후 동작</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>What to do after dictation</source>
        <translation>음성 입력을 마친 후 수행할 동작</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Paste to active window</source>
        <translation>활성 창에 붙여넣기</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Copy to clipboard</source>
        <translation>클립보드에 복사</translation>
    </message>
    <message>
        <location filename="../src/builtins/snippet/snippet-extension.hpp" line="+25"/>
        <source>Expansion</source>
        <translation>확장</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Enable automatic snippet expansion when triggers are typed</source>
        <translation>트리거를 입력하면 스니펫을 자동으로 확장합니다</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Undo</source>
        <translation>실행 취소</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Press backspace immediately after expansion to undo and restore the trigger text</source>
        <translation>확장 직후 백스페이스를 누르면 실행을 취소하고 트리거 텍스트를 복원합니다</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Keyboard layout</source>
        <translation>키보드 레이아웃</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>XKB layout used for trigger detection (e.g. &quot;us&quot;, &quot;fr&quot;). Leave empty for system default.</source>
        <translation>트리거 감지에 사용할 XKB 레이아웃입니다 (예: &quot;us&quot;, &quot;fr&quot;). 시스템 기본값을 사용하려면 비워 두세요.</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Pre-paste delay (ms)</source>
        <translation>붙여넣기 전 지연 (ms)</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Delay between setting clipboard and injecting paste shortcut. Increase if expansions paste empty on slow compositors.</source>
        <translation>클립보드를 설정한 뒤 붙여넣기 단축키를 입력하기까지의 지연 시간입니다. 느린 컴포지터에서 확장 시 빈 내용이 붙여넣어지면 늘리세요.</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Key injection delay (ms)</source>
        <translation>키 입력 지연 (ms)</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Delay between injected key events. Increase if expansions produce missing or garbled characters on slow compositors.</source>
        <translation>주입한 키 이벤트 사이의 지연 시간입니다. 느린 컴포지터에서 확장 시 문자가 누락되거나 깨지면 늘리세요.</translation>
    </message>
    <message>
        <location filename="../src/builtins/system/browse-apps-preferences.hpp" line="+13"/>
        <source>Sort alphabetically</source>
        <translation>이름순 정렬</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Show hidden apps</source>
        <translation>숨겨진 앱 표시</translation>
    </message>
    <message>
        <location filename="../src/builtins/vicinae/emoji-preferences.hpp" line="+5"/>
        <source>Skin tone</source>
        <translation>피부색</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Skin tone to use for relevant emojis.</source>
        <translation>지원하는 이모지에 사용할 피부색입니다.</translation>
    </message>
    <message>
        <location filename="../src/builtins/vicinae/store-intro-preferences.hpp" line="+12"/>
        <source>Always show intro</source>
        <translation>항상 소개 표시</translation>
    </message>
    <message>
        <location filename="../src/root-search/scripts/script-root-provider.hpp" line="+137"/>
        <source>Custom directories</source>
        <translation>사용자 지정 디렉터리</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Additional list of directories to source scripts from. These directories always take precedence over the default system ones</source>
        <translation>스크립트를 불러올 추가 디렉터리 목록입니다. 이 디렉터리는 항상 기본 시스템 디렉터리보다 우선합니다</translation>
    </message>
    <message>
        <location filename="../src/services/ai/ai-preferences.hpp" line="+52"/>
        <source>Providers</source>
        <translation>제공자</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Local and cloud AI providers. Their models power dictation and Quick AI.</source>
        <translation>로컬 및 클라우드 AI 제공자입니다. 제공자의 모델을 음성 입력과 Quick AI에 사용합니다.</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Default model</source>
        <translation>기본 모델</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>The model new conversations start with in AI Chat and Quick AI, also used by extensions that don&apos;t ask for a specific one.</source>
        <translation>AI Chat 및 Quick AI에서 새 대화를 시작할 때 사용할 모델입니다. 특정 모델을 요청하지 않는 확장 프로그램에서도 사용합니다.</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Custom instructions</source>
        <translation>맞춤 지침</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Tell the AI how to respond in Quick AI and AI Chat: your language, tone, how long answers should be, or anything it should know about you.</source>
        <translation>Quick AI 및 AI Chat의 응답 언어, 말투, 답변 길이 또는 알아 두어야 할 사용자 정보를 AI에 알려 주세요.</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Answer in French. Keep answers short unless I ask for details.</source>
        <translation>프랑스어로 답하세요. 자세한 설명을 요청하지 않으면 짧게 답하세요.</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Enable tools</source>
        <translation>도구 활성화</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Allow AI models to use tools from enabled extensions.</source>
        <translation>AI 모델이 활성화된 확장 프로그램의 도구를 사용할 수 있도록 허용합니다.</translation>
    </message>
    <message>
        <location filename="../src/services/ai/ai-provider-types.hpp" line="-164"/>
        <location line="+54"/>
        <location line="+38"/>
        <location line="+14"/>
        <location line="+14"/>
        <location line="+21"/>
        <source>Name</source>
        <translation>이름</translation>
    </message>
    <message>
        <location line="-140"/>
        <source>How this instance is shown in model lists.</source>
        <translation>모델 목록에 표시할 이 인스턴스의 이름입니다.</translation>
    </message>
    <message>
        <location line="+4"/>
        <location line="+54"/>
        <location line="+52"/>
        <location line="+14"/>
        <location line="+21"/>
        <source>Server URL</source>
        <translation>서버 URL</translation>
    </message>
    <message>
        <location line="-140"/>
        <source>The address of your Ollama instance.</source>
        <translation>Ollama 인스턴스의 주소입니다.</translation>
    </message>
    <message>
        <location line="+8"/>
        <location line="+10"/>
        <location line="+10"/>
        <location line="+10"/>
        <location line="+21"/>
        <location line="+65"/>
        <source>API Key</source>
        <translation>API 키</translation>
    </message>
    <message>
        <location line="-115"/>
        <source>Your OpenAI API key. You can create one in the OpenAI platform dashboard.</source>
        <translation>OpenAI API 키입니다. OpenAI 플랫폼 대시보드에서 만들 수 있습니다.</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Your Groq API key. You can create one in the Groq console.</source>
        <translation>Groq API 키입니다. Groq 콘솔에서 만들 수 있습니다.</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Your OpenRouter API key. You can create one in your OpenRouter settings.</source>
        <translation>OpenRouter API 키입니다. OpenRouter 설정에서 만들 수 있습니다.</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Your Mistral AI API key. You can find it in your Mistral dashboard.</source>
        <translation>Mistral AI API 키입니다. Mistral 대시보드에서 확인할 수 있습니다.</translation>
    </message>
    <message>
        <location line="+10"/>
        <location line="+52"/>
        <location line="+14"/>
        <location line="+21"/>
        <source>How this server is shown in model lists.</source>
        <translation>모델 목록에 표시할 이 서버의 이름입니다.</translation>
    </message>
    <message>
        <location line="-81"/>
        <source>Base URL of the OpenAI-compatible API, including the version prefix if the server has one.</source>
        <translation>OpenAI 호환 API의 기본 URL입니다. 서버에 버전 접두사가 있으면 함께 입력하세요.</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Leave empty if the server does not require one.</source>
        <translation>서버에서 요구하지 않으면 비워 두세요.</translation>
    </message>
    <message>
        <location line="+17"/>
        <location line="+14"/>
        <source>Command</source>
        <translation>명령</translation>
    </message>
    <message>
        <location line="-13"/>
        <source>Command that starts the agent in ACP mode. Leave empty to find it automatically.</source>
        <translation>ACP 모드로 에이전트를 시작하는 명령입니다. 자동으로 찾으려면 비워 두세요.</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>How this agent is shown in model lists.</source>
        <translation>모델 목록에 표시할 이 에이전트의 이름입니다.</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Command that starts the agent in ACP mode, with its arguments.</source>
        <translation>ACP 모드로 에이전트를 시작하는 명령과 인수입니다.</translation>
    </message>
    <message>
        <location line="+14"/>
        <source>The address of your llama-server instance.</source>
        <translation>llama-server 인스턴스의 주소입니다.</translation>
    </message>
    <message>
        <location line="+14"/>
        <source>The address of your Lemonade server.</source>
        <translation>Lemonade 서버의 주소입니다.</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Only needed if the server was started with an API key.</source>
        <translation>서버를 API 키와 함께 시작한 경우에만 필요합니다.</translation>
    </message>
    <message>
        <location line="+16"/>
        <source>The address of your audio.cpp server.</source>
        <translation>audio.cpp 서버의 주소입니다.</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Keep models loaded</source>
        <translation>모델 로드 유지</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>How long a model stays in memory after its last use.</source>
        <translation>마지막으로 사용한 뒤 모델을 메모리에 유지할 시간입니다.</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Unload immediately</source>
        <translation>즉시 해제</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>1 minute</source>
        <translation>1분</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>5 minutes</source>
        <translation>5분</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Until Vicinae quits</source>
        <translation>Vicinae 종료 시까지</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Run models on</source>
        <translation>모델 실행 장치</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Falls back to the CPU when no compatible GPU is available.</source>
        <translation>호환되는 GPU가 없으면 CPU를 사용합니다.</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>GPU</source>
        <translation>GPU</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>CPU</source>
        <translation>CPU</translation>
    </message>
    <message>
        <location filename="../src/services/app-service/app-preferences.hpp" line="+36"/>
        <source>Default action</source>
        <translation>기본 동작</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Action to perform when the return key is pressed. Always default to &apos;launch&apos; if the app has no open window.</source>
        <translation>Return을 눌렀을 때 수행할 동작입니다. 앱에 열린 창이 없으면 항상 &apos;앱 실행&apos;을 사용합니다.</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Focus window</source>
        <translation>창으로 전환</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Launch app</source>
        <translation>앱 실행</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Launch Prefix</source>
        <translation>실행 접두사</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Custom app launcher to use. Affects applications as well as their sub-actions.</source>
        <translation>사용할 사용자 지정 앱 실행기입니다. 앱과 해당 하위 동작에 모두 적용됩니다.</translation>
    </message>
    <message>
        <location line="+4"/>
        <location line="+9"/>
        <location line="+6"/>
        <source>Application directories</source>
        <translation>앱 디렉터리</translation>
    </message>
    <message>
        <location line="-14"/>
        <source>Directories applications are sourced from. The list cannot be modified directly. In order to do so, you need to append additonal paths to the &lt;b&gt;XDG_DATA_DIRS&lt;/b&gt; environment variables.</source>
        <translation>앱을 검색할 디렉터리입니다. 이 목록은 직접 수정할 수 없습니다. 변경하려면 &lt;b&gt;XDG_DATA_DIRS&lt;/b&gt; 환경 변수에 경로를 추가해야 합니다.</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Directories applications are sourced from.</source>
        <translation>앱을 검색할 디렉터리입니다.</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Directories applications are sourced from. System directories are always scanned and cannot be removed.</source>
        <translation>앱을 검색할 디렉터리입니다. 시스템 디렉터리는 항상 검색하며 제거할 수 없습니다.</translation>
    </message>
</context>
<context>
    <name>PreviewFontAction</name>
    <message>
        <location filename="../src/builtins/font/font-grid-model.cpp" line="-123"/>
        <source>Preview font</source>
        <translation>글꼴 미리보기</translation>
    </message>
</context>
<context>
    <name>PreviousTrackCommand</name>
    <message>
        <location filename="../src/builtins/media/media-extension.hpp" line="+43"/>
        <location line="+26"/>
        <source>Previous Track</source>
        <translation>이전 트랙</translation>
    </message>
    <message>
        <location line="-25"/>
        <source>Skip to the previous track</source>
        <translation>이전 트랙으로 건너뛰기</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>player</source>
        <translation>플레이어</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>%1 cannot skip to the previous track</source>
        <translation>%1에서 이전 트랙으로 건너뛸 수 없습니다</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Failed to skip to the previous track</source>
        <translation>이전 트랙으로 건너뛰지 못했습니다</translation>
    </message>
</context>
<context>
    <name>ProgramsSection</name>
    <message>
        <location filename="../src/builtins/system/system-run-model.cpp" line="+52"/>
        <source>Open in %1 (hold)</source>
        <translation>%1에서 열기 (길게 누르기)</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Open in %1</source>
        <translation>%1에서 열기</translation>
    </message>
    <message>
        <location line="+20"/>
        <source>Copy exec path</source>
        <translation>실행 파일 경로 복사</translation>
    </message>
    <message>
        <location filename="../src/builtins/system/system-run-model.hpp" line="+41"/>
        <source>Programs (%1)</source>
        <translation>프로그램 (%1)</translation>
    </message>
</context>
<context>
    <name>ProviderSearchSection</name>
    <message>
        <location filename="../src/builtins/root/provider-search-model.hpp" line="+11"/>
        <source>Results ({count})</source>
        <translation>결과 ({count})</translation>
    </message>
</context>
<context>
    <name>ProviderSearchViewHost</name>
    <message>
        <location filename="../src/builtins/root/provider-search-view-host.cpp" line="+15"/>
        <source>Search %1</source>
        <translation>%1 검색</translation>
    </message>
</context>
<context>
    <name>PutCalculatorAnswerInSearchBar</name>
    <message>
        <location filename="../src/actions/calculator-actions.hpp" line="-20"/>
        <source>Put answer in search bar</source>
        <translation>검색창에 답 넣기</translation>
    </message>
</context>
<context>
    <name>QObject</name>
    <message>
        <location filename="../src/utils/utils.cpp" line="+48"/>
        <source>in the future</source>
        <translation>미래</translation>
    </message>
    <message numerus="yes">
        <location line="+8"/>
        <source>%n year(s) ago</source>
        <translation>
            <numerusform>%n년 전</numerusform>
        </translation>
    </message>
    <message numerus="yes">
        <location line="+3"/>
        <source>%n month(s) ago</source>
        <translation>
            <numerusform>%n개월 전</numerusform>
        </translation>
    </message>
    <message numerus="yes">
        <location line="+2"/>
        <source>%n day(s) ago</source>
        <translation>
            <numerusform>%n일 전</numerusform>
        </translation>
    </message>
    <message numerus="yes">
        <location line="+2"/>
        <source>%n hour(s) ago</source>
        <translation>
            <numerusform>%n시간 전</numerusform>
        </translation>
    </message>
    <message numerus="yes">
        <location line="+2"/>
        <source>%n minute(s) ago</source>
        <translation>
            <numerusform>%n분 전</numerusform>
        </translation>
    </message>
    <message>
        <location line="+2"/>
        <source>just now</source>
        <translation>방금</translation>
    </message>
</context>
<context>
    <name>QuickAICommand</name>
    <message>
        <location filename="../src/builtins/ai/ai-extension.hpp" line="-23"/>
        <source>Quick AI</source>
        <translation>Quick AI</translation>
    </message>
    <message>
        <location filename="../src/builtins/ai/ai-extension.cpp" line="+19"/>
        <source>Set up an AI provider</source>
        <translation>AI 제공자 설정</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Quick AI needs a language model before it can answer. Add an AI provider in the settings.</source>
        <translation>Quick AI가 답하려면 언어 모델이 필요합니다. 설정에서 AI 제공자를 추가하세요.</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Open AI Settings</source>
        <translation>AI 설정 열기</translation>
    </message>
</context>
<context>
    <name>QuickAIView</name>
    <message>
        <location filename="../src/ui/qml/views/QuickAIView.qml" line="+54"/>
        <source>Open in AI Chat</source>
        <translation>AI Chat에서 열기</translation>
    </message>
    <message>
        <location line="+52"/>
        <source>Ask anything...</source>
        <translation>무엇이든 물어보세요...</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Ask a follow-up...</source>
        <translation>이어서 물어보세요...</translation>
    </message>
</context>
<context>
    <name>QuickAIViewHost</name>
    <message>
        <location filename="../src/builtins/ai/quick-ai-view-host.cpp" line="+82"/>
        <source>Could not load conversations.</source>
        <translation>대화를 불러올 수 없습니다.</translation>
    </message>
    <message>
        <location line="+44"/>
        <source>Quick AI</source>
        <translation>Quick AI</translation>
    </message>
</context>
<context>
    <name>QuitAppAction</name>
    <message>
        <location filename="../src/actions/app-actions.cpp" line="-108"/>
        <source>Quit Application</source>
        <translation>앱 종료</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Failed to quit %1</source>
        <translation>%1을 종료하지 못했습니다</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Quit %1</source>
        <translation>%1 종료</translation>
    </message>
</context>
<context>
    <name>RaycastCompatExtension</name>
    <message>
        <location filename="../src/builtins/raycast/raycast-compat-extension.hpp" line="+15"/>
        <source>Raycast compatibility features</source>
        <translation>Raycast 호환 기능</translation>
    </message>
</context>
<context>
    <name>RaycastStoreCommand</name>
    <message>
        <location filename="../src/builtins/raycast/raycast-store-command.hpp" line="+15"/>
        <source>Install compatible extensions from the Raycast store</source>
        <translation>Raycast store에서 호환 확장 프로그램 설치</translation>
    </message>
    <message>
        <location line="+14"/>
        <source>
# Welcome to the Raycast Extension Store

Vicinae provides direct integration with the official [Raycast store](https://www.raycast.com/store), allowing you to search and install Raycast extensions directly from Vicinae.
</source>
        <translation>
# Raycast Extension Store에 오신 것을 환영합니다

Vicinae는 공식 [Raycast store](https://www.raycast.com/store)와 직접 연동되어 Vicinae에서 Raycast 확장 프로그램을 검색하고 설치할 수 있습니다.
</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>
Each extension has a colored compatibility indicator showing how well it works on Linux.

Vicinae also has its own [extension store](vicinae://launch/core/store), which does not suffer from these limitations.
</source>
        <translation>
각 확장 프로그램에는 Linux에서 얼마나 잘 작동하는지 나타내는 색상별 호환성 표시가 있습니다.

Vicinae는 이러한 제약이 없는 자체 [Extension Store](vicinae://launch/core/store)도 제공합니다.
</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>
Vicinae also has its own [extension store](vicinae://launch/core/store).
</source>
        <translation>
Vicinae는 자체 [Extension Store](vicinae://launch/core/store)도 제공합니다.
</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Continue to store</source>
        <translation>스토어로 이동</translation>
    </message>
</context>
<context>
    <name>RaycastStoreDetailHost</name>
    <message>
        <location filename="../src/builtins/raycast/raycast-store-detail-host.cpp" line="+41"/>
        <source>Failed to load extension</source>
        <translation>확장 프로그램을 불러오지 못했습니다</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>The extension &quot;%1&quot; could not be loaded. It may not exist or the store may be unreachable.</source>
        <translation>확장 프로그램 &quot;%1&quot;을 불러올 수 없습니다. 존재하지 않거나 스토어에 연결할 수 없습니다.</translation>
    </message>
    <message>
        <location line="+27"/>
        <source>Extension Store - %1</source>
        <translation>Extension Store - %1</translation>
    </message>
    <message>
        <location line="+31"/>
        <source>This extension should be fully compatible.</source>
        <translation>이 확장 프로그램은 완전히 호환됩니다.</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>This extension works but has a few quirks.</source>
        <translation>이 확장 프로그램은 작동하지만 일부 문제가 있습니다.</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>This extension is not compatible.</source>
        <translation>이 확장 프로그램은 호환되지 않습니다.</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>No compatibility data is available for this extension.</source>
        <translation>이 확장 프로그램의 호환성 정보가 없습니다.</translation>
    </message>
    <message>
        <location line="+21"/>
        <source>No compatibility data is available — this extension may or may not work.</source>
        <translation>호환성 정보가 없습니다. 이 확장 프로그램은 작동하지 않을 수도 있습니다.</translation>
    </message>
    <message>
        <location line="+75"/>
        <source>Extension Store</source>
        <translation>Extension Store</translation>
    </message>
    <message>
        <location line="+17"/>
        <source>Report issue</source>
        <translation>문제 신고</translation>
    </message>
</context>
<context>
    <name>RaycastStoreSection</name>
    <message>
        <location filename="../src/builtins/raycast/raycast-store-model.cpp" line="+46"/>
        <source>Show details</source>
        <translation>세부 정보 표시</translation>
    </message>
</context>
<context>
    <name>RaycastStoreViewHost</name>
    <message>
        <location filename="../src/builtins/raycast/raycast-store-view-host.cpp" line="+36"/>
        <source>Browse Raycast extensions</source>
        <translation>Raycast 확장 프로그램 찾아보기</translation>
    </message>
    <message>
        <location line="+32"/>
        <source>Failed to fetch extensions</source>
        <translation>확장 프로그램을 가져오지 못했습니다</translation>
    </message>
    <message>
        <location line="+18"/>
        <source>Extensions</source>
        <translation>확장 프로그램</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Failed to search extensions</source>
        <translation>확장 프로그램을 검색하지 못했습니다</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Results</source>
        <translation>결과</translation>
    </message>
</context>
<context>
    <name>RebootCommand</name>
    <message>
        <location filename="../src/builtins/power-management/power-management-extension.cpp" line="+124"/>
        <source>Reboot System</source>
        <translation>시스템 다시 시작</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Reboot the system</source>
        <translation>시스템을 다시 시작합니다</translation>
    </message>
    <message>
        <location line="+12"/>
        <source>System can&apos;t reboot</source>
        <translation>시스템을 다시 시작할 수 없습니다</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Failed to reboot</source>
        <translation>다시 시작하지 못했습니다</translation>
    </message>
</context>
<context>
    <name>RebuildFileIndexCommand</name>
    <message>
        <location filename="../src/builtins/file/file-extension.hpp" line="-64"/>
        <source>Rebuild File Index</source>
        <translation>파일 인덱스 다시 생성</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Fully rebuild the file index. Running this manually can be useful if the file search feels particularly out of date.</source>
        <translation>파일 인덱스를 완전히 다시 생성합니다. 파일 검색 결과가 최신 상태와 크게 다르다면 수동으로 실행하는 것이 도움이 될 수 있습니다.</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>Are you sure?</source>
        <translation>계속할까요?</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Rebuilding the entire index can be time consuming and CPU intensive, depending on the number of files present in your home directory.</source>
        <translation>홈 디렉터리의 파일 수에 따라 전체 인덱스를 다시 생성하는 데 시간이 오래 걸리거나 CPU 사용량이 많아질 수 있습니다.</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Reset</source>
        <translation>초기화</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Index rebuild started...</source>
        <translation>인덱스 다시 생성을 시작했습니다...</translation>
    </message>
</context>
<context>
    <name>RefreshAppsCommand</name>
    <message>
        <location filename="../src/builtins/vicinae/refresh-apps-command.cpp" line="+15"/>
        <source>Apps successfully refreshed</source>
        <translation>앱 목록을 새로고침했습니다</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Failed to refresh apps</source>
        <translation>앱 목록을 새로고침하지 못했습니다</translation>
    </message>
    <message>
        <location filename="../src/builtins/vicinae/refresh-apps-command.hpp" line="+12"/>
        <source>Refresh Apps</source>
        <translation>앱 목록 새로고침</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Force a refresh of the application database. The database should normally automatically update itself on changes, but this can help working around some edge cases.</source>
        <translation>앱 데이터베이스를 강제로 새로고침합니다. 일반적으로 변경 사항이 자동 반영되지만 일부 예외적인 상황을 해결하는 데 도움이 될 수 있습니다.</translation>
    </message>
</context>
<context>
    <name>RegisteredTool</name>
    <message>
        <location filename="../src/services/ai/tool-registry.cpp" line="+43"/>
        <source>This tool is not enabled.</source>
        <translation>이 도구는 활성화되어 있지 않습니다.</translation>
    </message>
</context>
<context>
    <name>ReloadScriptDirectoriesCommand</name>
    <message>
        <location filename="../src/builtins/vicinae/vicinae-extension.cpp" line="+76"/>
        <source>Reload Script Directories</source>
        <translation>스크립트 디렉터리 다시 불러오기</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Reload script directories</source>
        <translation>스크립트 디렉터리 다시 불러오기</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>New scan triggered, index will update shortly</source>
        <translation>새 검색을 시작했습니다. 곧 인덱스가 갱신됩니다</translation>
    </message>
</context>
<context>
    <name>RemoveAllCalculatorHistoryRecordsAction</name>
    <message>
        <location filename="../src/actions/calculator-actions.hpp" line="+76"/>
        <source>Delete all entries</source>
        <translation>모든 항목 삭제</translation>
    </message>
</context>
<context>
    <name>RemoveAllSelectionsAction</name>
    <message>
        <location filename="../src/builtins/clipboard/history/clipboard-history-actions.hpp" line="+28"/>
        <source>Are you sure?</source>
        <translation>계속할까요?</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>All your clipboard history will be lost forever</source>
        <translation>전체 클립보드 기록이 영구적으로 삭제됩니다</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Delete all</source>
        <translation>모두 삭제</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>All selections were removed</source>
        <translation>모든 선택 항목을 삭제했습니다</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Failed to remove all selections</source>
        <translation>모든 선택 항목을 삭제하지 못했습니다</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Remove all</source>
        <translation>모두 삭제</translation>
    </message>
</context>
<context>
    <name>RemoveCalculatorHistoryRecordAction</name>
    <message>
        <location filename="../src/actions/calculator-actions.hpp" line="-22"/>
        <source>Entry removed</source>
        <translation>항목을 삭제했습니다</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Failed to remove entry</source>
        <translation>항목을 삭제하지 못했습니다</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Delete entry</source>
        <translation>항목 삭제</translation>
    </message>
</context>
<context>
    <name>RemoveSelectionAction</name>
    <message>
        <location filename="../src/builtins/clipboard/history/clipboard-history-actions.hpp" line="-71"/>
        <source>Entry removed</source>
        <translation>항목을 삭제했습니다</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Failed to remove entry</source>
        <translation>항목을 삭제하지 못했습니다</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Remove entry</source>
        <translation>항목 삭제</translation>
    </message>
</context>
<context>
    <name>RemoveShortcutAction</name>
    <message>
        <location filename="../src/actions/shortcut-actions.hpp" line="+33"/>
        <source>Removed link</source>
        <translation>링크를 삭제했습니다</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Failed to remove link</source>
        <translation>링크를 삭제하지 못했습니다</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Remove link</source>
        <translation>링크 삭제</translation>
    </message>
</context>
<context>
    <name>ReportVicinaeBugCommand</name>
    <message>
        <location filename="../src/builtins/vicinae/report-bug-command.hpp" line="+10"/>
        <source>Report a Vicinae Bug</source>
        <translation>Vicinae 버그 신고</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Navigate to Vicinae issue creation page with all relevant informations pre-filled.</source>
        <translation>필요한 정보가 미리 입력된 Vicinae 이슈 작성 페이지로 이동합니다.</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Title</source>
        <translation>제목</translation>
    </message>
</context>
<context>
    <name>ResetEmojiRankingAction</name>
    <message>
        <location filename="../src/builtins/vicinae/emoji-grid-model.cpp" line="+21"/>
        <source>Reset ranking</source>
        <translation>순위 초기화</translation>
    </message>
</context>
<context>
    <name>ResetEmojiSkinToneAction</name>
    <message>
        <location line="+34"/>
        <source>Reset to preference</source>
        <translation>환경설정 값으로 초기화</translation>
    </message>
</context>
<context>
    <name>ResetItemRanking</name>
    <message>
        <location filename="../src/actions/root-search-actions.cpp" line="-69"/>
        <source>Ranking was successfully reset</source>
        <translation>순위를 초기화했습니다</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Unable to reset ranking</source>
        <translation>순위를 초기화할 수 없습니다</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Are you sure?</source>
        <translation>계속할까요?</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>You will have to rebuild search history for this item in order for it to reappear on top of the root search results.</source>
        <translation>이 항목이 메인 화면 검색 결과 상단에 다시 표시되려면 검색 기록을 새로 쌓아야 합니다.</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Reset</source>
        <translation>초기화</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Reset ranking</source>
        <translation>순위 초기화</translation>
    </message>
</context>
<context>
    <name>RevealFileInFolderAction</name>
    <message>
        <location filename="../src/utils/file-list-item.hpp" line="+31"/>
        <source>Show in file browser</source>
        <translation>파일 관리자에서 보기</translation>
    </message>
    <message>
        <location line="+12"/>
        <source>Failed to open folder</source>
        <translation>폴더를 열지 못했습니다</translation>
    </message>
</context>
<context>
    <name>RootCalculatorSection</name>
    <message>
        <location filename="../src/builtins/root/root-search-sources.cpp" line="+175"/>
        <source>Copy unformatted answer</source>
        <translation>서식 없이 답 복사</translation>
    </message>
    <message>
        <location filename="../src/builtins/root/root-search-sources.hpp" line="+89"/>
        <source>Calculator</source>
        <translation>계산기</translation>
    </message>
</context>
<context>
    <name>RootFallbackSection</name>
    <message>
        <location filename="../src/builtins/root/root-search-sources.cpp" line="+259"/>
        <source>Use &quot;%1&quot; with...</source>
        <translation>&quot;%1&quot;을 다음으로 사용...</translation>
    </message>
</context>
<context>
    <name>RootFavoritesSection</name>
    <message>
        <location filename="../src/builtins/root/root-search-sources.hpp" line="+67"/>
        <source>Favorites</source>
        <translation>즐겨찾기</translation>
    </message>
</context>
<context>
    <name>RootFilesSection</name>
    <message>
        <location line="+52"/>
        <source>Files</source>
        <translation>파일</translation>
    </message>
</context>
<context>
    <name>RootLinkSection</name>
    <message>
        <location filename="../src/builtins/root/root-search-sources.cpp" line="-311"/>
        <source>Open in %1</source>
        <translation>%1에서 열기</translation>
    </message>
    <message>
        <location filename="../src/builtins/root/root-search-sources.hpp" line="-141"/>
        <source>Link</source>
        <translation>링크</translation>
    </message>
</context>
<context>
    <name>RootNewsSection</name>
    <message>
        <location line="+66"/>
        <source>What&apos;s New</source>
        <translation>새로운 소식</translation>
    </message>
</context>
<context>
    <name>RootResultsSection</name>
    <message>
        <location filename="../src/builtins/root/root-search-sources.cpp" line="+204"/>
        <source>Suggestions</source>
        <translation>추천</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Results (%1)</source>
        <translation>결과 (%1)</translation>
    </message>
</context>
<context>
    <name>RootSearchActionGenerator</name>
    <message>
        <location filename="../src/actions/root-search-actions.hpp" line="+71"/>
        <source>Copy ID</source>
        <translation>ID 복사</translation>
    </message>
</context>
<context>
    <name>RootShortcutItem</name>
    <message>
        <location filename="../src/root-search/shortcuts/shortcut-root-provider.cpp" line="+70"/>
        <location line="+11"/>
        <source>Shortcut</source>
        <translation>바로가기</translation>
    </message>
</context>
<context>
    <name>RootUpdateSection</name>
    <message>
        <location filename="../src/builtins/root/root-search-sources.cpp" line="-137"/>
        <source>Vicinae %1 is available</source>
        <translation>Vicinae %1을 사용할 수 있습니다</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>You are running %1</source>
        <translation>현재 %1을 사용 중입니다</translation>
    </message>
    <message>
        <location line="+8"/>
        <location filename="../src/builtins/root/root-search-sources.hpp" line="-21"/>
        <source>Update</source>
        <translation>업데이트</translation>
    </message>
    <message>
        <location line="+18"/>
        <source>View Release Notes</source>
        <translation>릴리스 노트 보기</translation>
    </message>
</context>
<context>
    <name>RootViewHost</name>
    <message>
        <location filename="../src/builtins/root/root-view-host.hpp" line="+15"/>
        <source>Search for anything...</source>
        <translation>무엇이든 검색하세요...</translation>
    </message>
</context>
<context>
    <name>RunAppleShortcutAction</name>
    <message>
        <location filename="../src/root-search/apple-shortcuts/apple-shortcut-root-provider.cpp" line="-29"/>
        <source>Run Shortcut</source>
        <translation>단축어 실행</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Failed to start shortcut</source>
        <translation>단축어를 실행하지 못했습니다</translation>
    </message>
</context>
<context>
    <name>RunExecutableAction</name>
    <message>
        <location filename="../src/utils/file-list-item.hpp" line="+50"/>
        <source>Run executable</source>
        <translation>실행 파일 실행</translation>
    </message>
    <message>
        <location line="+12"/>
        <source>Failed to give executable permission</source>
        <translation>실행 권한을 부여하지 못했습니다</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Failed to start executable</source>
        <translation>실행 파일을 실행하지 못했습니다</translation>
    </message>
</context>
<context>
    <name>SaveAsAction</name>
    <message>
        <location filename="../src/actions/save-as-action.cpp" line="+36"/>
        <source>Save as…</source>
        <translation>다른 이름으로 저장…</translation>
    </message>
    <message>
        <location line="+27"/>
        <source>Saved %1</source>
        <translation>%1을 저장했습니다</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Failed to save %1</source>
        <translation>%1을 저장하지 못했습니다</translation>
    </message>
</context>
<context>
    <name>SaveFileFallback</name>
    <message>
        <location filename="../src/ui/qml/launcher/SaveFileFallback.qml" line="+8"/>
        <source>Save as</source>
        <translation>다른 이름으로 저장</translation>
    </message>
</context>
<context>
    <name>ScreenshotActions</name>
    <message>
        <location filename="../src/builtins/screenshots/screenshot-actions.cpp" line="+15"/>
        <source>Refresh</source>
        <translation>새로고침</translation>
    </message>
    <message>
        <location line="+13"/>
        <source>Cannot paste</source>
        <translation>붙여넣을 수 없음</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Allow Accessibility access to paste into other apps.</source>
        <translation>다른 앱에 붙여넣으려면 손쉬운 사용 권한을 허용하세요.</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Could not read recording</source>
        <translation>녹화 파일을 읽을 수 없습니다</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Could not read screenshot</source>
        <translation>스크린샷을 읽을 수 없습니다</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>The file may have been moved or deleted.</source>
        <translation>파일이 이동되었거나 삭제되었을 수 있습니다.</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Could not paste recording</source>
        <translation>녹화 파일을 붙여넣을 수 없습니다</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Could not paste screenshot</source>
        <translation>스크린샷을 붙여넣을 수 없습니다</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Recording copied</source>
        <translation>녹화 파일을 복사했습니다</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Screenshot copied</source>
        <translation>스크린샷을 복사했습니다</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Could not copy recording</source>
        <translation>녹화 파일을 복사할 수 없습니다</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Could not copy screenshot</source>
        <translation>스크린샷을 복사할 수 없습니다</translation>
    </message>
    <message>
        <location line="+12"/>
        <source>No saved screenshots found</source>
        <translation>저장된 스크린샷을 찾을 수 없습니다</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Take a screenshot and save it to a file first.</source>
        <translation>먼저 스크린샷을 찍어 파일로 저장하세요.</translation>
    </message>
    <message>
        <location line="+12"/>
        <source>Refreshing...</source>
        <translation>새로고침 중...</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Could not refresh all screenshots and recordings</source>
        <translation>일부 스크린샷 또는 녹화 파일을 새로고침할 수 없습니다</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Refreshed</source>
        <translation>새로고침했습니다</translation>
    </message>
    <message>
        <location line="+13"/>
        <source>Paste Recording</source>
        <translation>녹화 파일 붙여넣기</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Paste Screenshot</source>
        <translation>스크린샷 붙여넣기</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Copy Recording</source>
        <translation>녹화 파일 복사</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Copy Screenshot</source>
        <translation>스크린샷 복사</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Open Recording</source>
        <translation>녹화 파일 열기</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Open Screenshot</source>
        <translation>스크린샷 열기</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Could not open file</source>
        <translation>파일을 열 수 없습니다</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Show in Finder</source>
        <translation>Finder에서 보기</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Show in File Browser</source>
        <translation>파일 관리자에서 보기</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Could not show file in file browser</source>
        <translation>파일 관리자에서 파일을 표시할 수 없습니다</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Copy File Path</source>
        <translation>파일 경로 복사</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Move to Trash</source>
        <translation>휴지통으로 이동</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>File moved to Trash</source>
        <translation>파일을 휴지통으로 이동했습니다</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Could not move file to Trash</source>
        <translation>파일을 휴지통으로 이동할 수 없습니다</translation>
    </message>
</context>
<context>
    <name>ScreenshotGridModel</name>
    <message>
        <location filename="../src/builtins/screenshots/screenshot-grid-model.cpp" line="+80"/>
        <source>Today</source>
        <translation>오늘</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Yesterday</source>
        <translation>어제</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Older</source>
        <translation>이전</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>Search Results</source>
        <translation>검색 결과</translation>
    </message>
</context>
<context>
    <name>ScreenshotGridSource</name>
    <message>
        <location line="-79"/>
        <source>Screen Recording</source>
        <translation>화면 녹화</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Recording · %1</source>
        <translation>녹화 · %1</translation>
    </message>
</context>
<context>
    <name>ScreenshotsExtension</name>
    <message>
        <location filename="../src/builtins/screenshots/screenshots-extension.hpp" line="+17"/>
        <source>Screenshots</source>
        <translation>스크린샷</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Search and share saved screenshots and screen recordings.</source>
        <translation>저장된 스크린샷과 화면 녹화를 검색하고 공유합니다.</translation>
    </message>
</context>
<context>
    <name>ScreenshotsView</name>
    <message>
        <location filename="../src/ui/qml/views/ScreenshotsView.qml" line="+10"/>
        <source>No screenshots or recordings found</source>
        <translation>스크린샷이나 녹화 파일을 찾을 수 없습니다</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Save a screenshot or screen recording to a file, or try another search.</source>
        <translation>스크린샷이나 화면 녹화를 파일로 저장하거나 다른 검색어를 입력하세요.</translation>
    </message>
</context>
<context>
    <name>ScreenshotsViewHost</name>
    <message>
        <location filename="../src/builtins/screenshots/screenshots-view-host.cpp" line="+13"/>
        <source>Search screenshots and recordings...</source>
        <translation>스크린샷 및 녹화 검색...</translation>
    </message>
</context>
<context>
    <name>ScriptExecutorViewHost</name>
    <message>
        <location filename="../src/script/script-executor-view-host.cpp" line="+79"/>
        <source>Script execution failed: %1</source>
        <translation>스크립트 실행 실패: %1</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Running... (%1s ago)</source>
        <translation>실행 중... (%1초 경과)</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Done in %1s (exit=%2)</source>
        <translation>%1초 만에 완료 (종료 코드=%2)</translation>
    </message>
    <message>
        <location line="+12"/>
        <location line="+9"/>
        <source>Script process killed</source>
        <translation>스크립트 프로세스가 종료되었습니다</translation>
    </message>
    <message>
        <location line="+20"/>
        <source>Running...</source>
        <translation>실행 중...</translation>
    </message>
    <message>
        <location line="+15"/>
        <source>Kill process</source>
        <translation>프로세스 강제 종료</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Run script again</source>
        <translation>스크립트 다시 실행</translation>
    </message>
</context>
<context>
    <name>ScriptRootItem</name>
    <message>
        <location filename="../src/root-search/scripts/script-root-provider.hpp" line="-112"/>
        <location line="+87"/>
        <source>Script</source>
        <translation>스크립트</translation>
    </message>
    <message>
        <location line="-44"/>
        <source>Mode</source>
        <translation>모드</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Path</source>
        <translation>경로</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Author</source>
        <translation>작성자</translation>
    </message>
    <message>
        <location line="+23"/>
        <source>Open script directory</source>
        <translation>스크립트 디렉터리 열기</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Copy path to script</source>
        <translation>스크립트 경로 복사</translation>
    </message>
</context>
<context>
    <name>ScriptRootProvider</name>
    <message>
        <location line="+64"/>
        <source>Script Commands</source>
        <translation>스크립트 명령</translation>
    </message>
</context>
<context>
    <name>SearchBar</name>
    <message>
        <location filename="../src/ui/qml/launcher/SearchBar.qml" line="+368"/>
        <source>Ask AI</source>
        <translation>AI에게 질문</translation>
    </message>
</context>
<context>
    <name>SearchBrowserBookmarksCommand</name>
    <message>
        <location filename="../src/builtins/browser/browser-extension.cpp" line="+31"/>
        <source>Search Browser Bookmarks</source>
        <translation>브라우저 북마크 검색</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Search bookmarks from all connected browsers</source>
        <translation>연결된 모든 브라우저의 북마크를 검색합니다</translation>
    </message>
</context>
<context>
    <name>SearchBrowserHistoryCommand</name>
    <message>
        <location line="+15"/>
        <source>Search Browser History</source>
        <translation>브라우저 방문 기록 검색</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Search browsing history from all connected browsers</source>
        <translation>연결된 모든 브라우저의 방문 기록을 검색합니다</translation>
    </message>
</context>
<context>
    <name>SearchBrowserTabsCommand</name>
    <message>
        <location line="-33"/>
        <source>Search Browser Tabs</source>
        <translation>브라우저 탭 검색</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Search tabs from all connected browsers</source>
        <translation>연결된 모든 브라우저의 탭을 검색합니다</translation>
    </message>
</context>
<context>
    <name>SearchEmojiCommand</name>
    <message>
        <location filename="../src/builtins/vicinae/search-emoji-command.hpp" line="+16"/>
        <source>Search Emojis &amp; Symbols</source>
        <translation>이모지 및 기호 검색</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Search for any emoji or symbol</source>
        <translation>이모지나 기호를 검색합니다</translation>
    </message>
</context>
<context>
    <name>SearchEmojiGridSource</name>
    <message>
        <location filename="../src/builtins/vicinae/emoji-grid-model.hpp" line="-20"/>
        <source>Results (%1)</source>
        <translation>결과 (%1)</translation>
    </message>
</context>
<context>
    <name>SearchFilesCommand</name>
    <message>
        <location filename="../src/builtins/file/file-extension.hpp" line="-34"/>
        <source>Search Files</source>
        <translation>파일 검색</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Search files on your system</source>
        <translation>시스템의 파일을 검색합니다</translation>
    </message>
</context>
<context>
    <name>SearchFilesView</name>
    <message>
        <location filename="../src/ui/qml/views/SearchFilesView.qml" line="+37"/>
        <source>Name</source>
        <translation>이름</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Path</source>
        <translation>경로</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Type</source>
        <translation>유형</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Last modified</source>
        <translation>마지막 수정</translation>
    </message>
</context>
<context>
    <name>SearchFilesViewHost</name>
    <message>
        <location filename="../src/builtins/file/search-files-view-host.cpp" line="+55"/>
        <source>Search for files...</source>
        <translation>파일 검색...</translation>
    </message>
    <message>
        <location line="+31"/>
        <location line="+4"/>
        <source>Direct file path</source>
        <translation>직접 입력한 파일 경로</translation>
    </message>
    <message>
        <location line="+38"/>
        <source>Recently Accessed</source>
        <translation>최근 접근</translation>
    </message>
    <message>
        <location line="+43"/>
        <source>Recently Modified</source>
        <translation>최근 수정</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Results</source>
        <translation>결과</translation>
    </message>
    <message>
        <location line="+40"/>
        <source>All</source>
        <translation>전체</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Other</source>
        <translation>기타</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Directories</source>
        <translation>디렉터리</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Images</source>
        <translation>이미지</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Videos</source>
        <translation>동영상</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Audio</source>
        <translation>오디오</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Documents</source>
        <translation>문서</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Archives</source>
        <translation>압축 파일</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Applications</source>
        <translation>앱</translation>
    </message>
</context>
<context>
    <name>SearchMenuBarCommand</name>
    <message>
        <location filename="../src/builtins/vicinae/search-menu-bar-command.hpp" line="+11"/>
        <source>Search Menu Bar Items</source>
        <translation>메뉴 막대 항목 검색</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Search and run menu bar items of the frontmost application</source>
        <translation>맨 앞에 있는 앱의 메뉴 막대 항목을 검색하고 실행합니다</translation>
    </message>
</context>
<context>
    <name>SearchScreenshotsCommand</name>
    <message>
        <location filename="../src/builtins/screenshots/screenshots-extension.hpp" line="-38"/>
        <source>Search Screenshots</source>
        <translation>스크린샷 검색</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Search and share saved screenshots and screen recordings.</source>
        <translation>저장된 스크린샷과 화면 녹화를 검색하고 공유합니다.</translation>
    </message>
</context>
<context>
    <name>SearchTrayCommand</name>
    <message>
        <location filename="../src/builtins/vicinae/search-tray-command.hpp" line="+10"/>
        <source>Search Tray</source>
        <translation>트레이 검색</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Browse system tray items and trigger their menu actions</source>
        <translation>시스템 트레이 항목을 살펴보고 메뉴 동작을 실행합니다</translation>
    </message>
</context>
<context>
    <name>SearchTrayViewHost</name>
    <message>
        <location filename="../src/builtins/vicinae/search-tray-view-host.hpp" line="+122"/>
        <source>Search tray items...</source>
        <translation>트레이 항목 검색...</translation>
    </message>
    <message>
        <location line="+16"/>
        <source>Attention</source>
        <translation>확인 필요</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Browse Menu</source>
        <translation>메뉴 살펴보기</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Activate</source>
        <translation>실행</translation>
    </message>
    <message>
        <location line="+15"/>
        <source>Secondary Activate</source>
        <translation>보조 동작 실행</translation>
    </message>
</context>
<context>
    <name>SetAppFont</name>
    <message>
        <location filename="../src/builtins/font/font-grid-model.cpp" line="-15"/>
        <source>Set as vicinae font</source>
        <translation>Vicinae 글꼴로 설정</translation>
    </message>
</context>
<context>
    <name>SetDefaultBrowser</name>
    <message>
        <location filename="../src/builtins/system/system-extension.hpp" line="+119"/>
        <source>Set Default Browser</source>
        <translation>기본 브라우저 설정</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Change the default system web browser</source>
        <translation>시스템 기본 웹 브라우저를 변경합니다</translation>
    </message>
</context>
<context>
    <name>SetDefaultBrowserViewHost</name>
    <message>
        <location filename="../src/builtins/system/set-default-browser-view-host.hpp" line="+22"/>
        <source>Select a web browser...</source>
        <translation>웹 브라우저 선택...</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Available web browsers</source>
        <translation>사용 가능한 웹 브라우저</translation>
    </message>
    <message>
        <location line="+21"/>
        <source>Set as default browser</source>
        <translation>기본 브라우저로 설정</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Default browser changed</source>
        <translation>기본 브라우저를 변경했습니다</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Failed to set default browser</source>
        <translation>기본 브라우저를 설정하지 못했습니다</translation>
    </message>
</context>
<context>
    <name>SetDefaultTerminal</name>
    <message>
        <location filename="../src/builtins/system/system-extension.hpp" line="-11"/>
        <source>Set Default Terminal</source>
        <translation>기본 터미널 설정</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Change the default system terminal</source>
        <translation>시스템 기본 터미널을 변경합니다</translation>
    </message>
</context>
<context>
    <name>SetRootItemAliasAction</name>
    <message>
        <location filename="../src/actions/root-search-actions.hpp" line="-34"/>
        <source>Set alias</source>
        <translation>별칭 설정</translation>
    </message>
</context>
<context>
    <name>SetRootItemShortcutAction</name>
    <message>
        <location filename="../src/actions/root-search-actions.cpp" line="+103"/>
        <source>Set Global Shortcut</source>
        <translation>전역 단축키 설정</translation>
    </message>
</context>
<context>
    <name>SetThemeAction</name>
    <message>
        <location filename="../src/actions/theme-actions.cpp" line="+11"/>
        <source>Theme successfully updated</source>
        <translation>테마를 변경했습니다</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Set theme</source>
        <translation>테마 설정</translation>
    </message>
</context>
<context>
    <name>SetThemeCommand</name>
    <message>
        <location filename="../src/builtins/theme/set-theme-command.hpp" line="+9"/>
        <source>Set Theme</source>
        <translation>테마 설정</translation>
    </message>
</context>
<context>
    <name>SetVolumeCommand</name>
    <message>
        <location filename="../src/builtins/media/media-extension.hpp" line="+111"/>
        <source>Set Volume to %1%</source>
        <translation>음량을 %1%로 설정</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Set system volume to %1%</source>
        <translation>시스템 음량을 %1%로 설정</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Failed to set volume</source>
        <translation>음량을 설정하지 못했습니다</translation>
    </message>
</context>
<context>
    <name>SetWallpaperAction</name>
    <message>
        <location filename="../src/utils/file-list-item.hpp" line="-55"/>
        <source>Set as wallpaper</source>
        <translation>배경화면으로 설정</translation>
    </message>
    <message>
        <location line="+12"/>
        <source>Wallpaper set</source>
        <translation>배경화면을 설정했습니다</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Failed to set wallpaper</source>
        <translation>배경화면을 설정하지 못했습니다</translation>
    </message>
</context>
<context>
    <name>SettingsPreferenceForm</name>
    <message>
        <location filename="../src/ui/qml/settings/SettingsPreferenceForm.qml" line="+105"/>
        <source>Edit</source>
        <translation>편집</translation>
    </message>
    <message>
        <location line="+93"/>
        <source>Select an app…</source>
        <translation>앱 선택…</translation>
    </message>
    <message>
        <location line="+45"/>
        <source>Add app…</source>
        <translation>앱 추가…</translation>
    </message>
</context>
<context>
    <name>SettingsSidebar</name>
    <message>
        <location filename="../src/ui/qml/settings/SettingsSidebar.qml" line="+129"/>
        <source>Search...</source>
        <translation>검색...</translation>
    </message>
</context>
<context>
    <name>SettingsSidebarModel</name>
    <message>
        <location filename="../src/ui/settings/settings-sidebar-model.cpp" line="+94"/>
        <source>General</source>
        <translation>일반</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Appearance</source>
        <translation>모양</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Keybindings</source>
        <translation>키 바인딩</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Advanced</source>
        <translation>고급</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>About</source>
        <translation>정보</translation>
    </message>
</context>
<context>
    <name>SettingsSubpageRow</name>
    <message>
        <location filename="../src/ui/qml/settings/SettingsSubpageRow.qml" line="+16"/>
        <source>Manage</source>
        <translation>관리</translation>
    </message>
</context>
<context>
    <name>SettingsWindow</name>
    <message>
        <location filename="../src/ui/qml/settings/SettingsWindow.qml" line="+13"/>
        <source>General</source>
        <translation>일반</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Appearance</source>
        <translation>모양</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Keybindings</source>
        <translation>키 바인딩</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Advanced</source>
        <translation>고급</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>About</source>
        <translation>정보</translation>
    </message>
    <message>
        <location line="+30"/>
        <source>Vicinae Settings</source>
        <translation>Vicinae 설정</translation>
    </message>
    <message>
        <location line="+218"/>
        <source>Imported from Raycast</source>
        <translation>Raycast에서 가져옴</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>From the Vicinae store</source>
        <translation>Vicinae store에서 설치</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Locally installed extension</source>
        <translation>로컬로 설치한 확장 프로그램</translation>
    </message>
</context>
<context>
    <name>ShortcutExtension</name>
    <message>
        <location filename="../src/builtins/shortcut/shortcut-extension.hpp" line="+11"/>
        <source>Manage Shortcuts</source>
        <translation>바로가기 관리</translation>
    </message>
</context>
<context>
    <name>ShortcutField</name>
    <message>
        <location filename="../src/ui/qml/form/ShortcutField.qml" line="+16"/>
        <source>Record shortcut</source>
        <translation>단축키 기록</translation>
    </message>
</context>
<context>
    <name>ShortcutFormView</name>
    <message>
        <location filename="../src/ui/qml/views/ShortcutFormView.qml" line="+15"/>
        <source>Name</source>
        <translation>이름</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Shortcut Name</source>
        <translation>바로가기 이름</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>The URL that will be opened by the specified app. You can make it dynamic by using placeholders such as {argument}.</source>
        <translation>지정한 앱에서 열 URL입니다. {argument} 같은 자리표시자로 동적 URL을 만들 수 있습니다.</translation>
    </message>
    <message>
        <location line="+23"/>
        <source>Open with</source>
        <translation>연결 프로그램</translation>
    </message>
    <message>
        <location line="+13"/>
        <source>Icon</source>
        <translation>아이콘</translation>
    </message>
</context>
<context>
    <name>ShortcutFormViewHost</name>
    <message>
        <location filename="../src/builtins/shortcut/shortcut-form-view-host.cpp" line="+46"/>
        <source>Submit</source>
        <translation>제출</translation>
    </message>
    <message>
        <location line="+13"/>
        <source>Copy of %1</source>
        <translation>%1의 복사본</translation>
    </message>
    <message>
        <location line="+45"/>
        <source>Edit &quot;%1&quot;</source>
        <translation>&quot;%1&quot; 편집</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Duplicate &quot;%1&quot;</source>
        <translation>&quot;%1&quot; 복제</translation>
    </message>
    <message>
        <location line="+9"/>
        <location line="+109"/>
        <location line="+43"/>
        <source>Default</source>
        <translation>기본값</translation>
    </message>
    <message>
        <location line="-136"/>
        <source>Selected Text</source>
        <translation>선택한 텍스트</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Clipboard Text</source>
        <translation>클립보드 텍스트</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Argument</source>
        <translation>인수</translation>
    </message>
    <message>
        <location line="+25"/>
        <location line="+5"/>
        <location line="+5"/>
        <source>Required</source>
        <translation>필수</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Validation failed</source>
        <translation>유효성 검사 실패</translation>
    </message>
    <message>
        <location line="+12"/>
        <source>Failed to update shortcut</source>
        <translation>바로가기를 수정하지 못했습니다</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Shortcut updated</source>
        <translation>바로가기를 수정했습니다</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Failed to create shortcut</source>
        <translation>바로가기를 만들지 못했습니다</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Shortcut created</source>
        <translation>바로가기를 만들었습니다</translation>
    </message>
</context>
<context>
    <name>ShortcutRecorderCapture</name>
    <message>
        <location filename="../src/ui/qml/form/ShortcutRecorderCapture.qml" line="+28"/>
        <location line="+14"/>
        <location line="+81"/>
        <source>Recording...</source>
        <translation>기록 중...</translation>
    </message>
    <message>
        <location line="+30"/>
        <source>Keybind updated</source>
        <translation>키 바인딩을 변경했습니다</translation>
    </message>
</context>
<context>
    <name>ShortcutRecorderPanel</name>
    <message>
        <location filename="../src/ui/qml/actions/ShortcutRecorderPanel.qml" line="+108"/>
        <source>Press Backspace to remove the current shortcut</source>
        <translation>현재 단축키를 제거하려면 백스페이스를 누르세요</translation>
    </message>
</context>
<context>
    <name>ShortcutRootProvider</name>
    <message>
        <location filename="../src/root-search/shortcuts/shortcut-root-provider.cpp" line="+42"/>
        <source>Shortcuts</source>
        <translation>바로가기</translation>
    </message>
</context>
<context>
    <name>ShortcutsSettingsPage</name>
    <message>
        <location filename="../src/ui/qml/settings/ShortcutsSettingsPage.qml" line="+61"/>
        <source>Keybindings</source>
        <translation>키 바인딩</translation>
    </message>
    <message>
        <location line="+78"/>
        <source>Record Shortcut</source>
        <translation>단축키 기록</translation>
    </message>
</context>
<context>
    <name>SkipUpdateVersionAction</name>
    <message>
        <location filename="../src/services/update/update-service.cpp" line="+8"/>
        <source>Skip This Version</source>
        <translation>이 버전 건너뛰기</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Skipped %1</source>
        <translation>%1을 건너뛰었습니다</translation>
    </message>
</context>
<context>
    <name>SleepCommand</name>
    <message>
        <location filename="../src/builtins/power-management/power-management-extension.cpp" line="+94"/>
        <source>Put System to Sleep</source>
        <translation>절전 모드로 전환</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Put system to sleep</source>
        <translation>시스템을 절전 모드로 전환합니다</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>System can&apos;t sleep</source>
        <translation>절전 모드로 전환할 수 없습니다</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Failed to sleep</source>
        <translation>절전 모드로 전환하지 못했습니다</translation>
    </message>
</context>
<context>
    <name>SnippetDatabase</name>
    <message>
        <location filename="../src/services/snippet/snippet-db.cpp" line="+76"/>
        <location line="+87"/>
        <source>keyword already assigned to &quot;%1&quot;</source>
        <translation>이미 &quot;%1&quot;에 지정된 키워드입니다</translation>
    </message>
    <message>
        <location line="-80"/>
        <location line="+87"/>
        <source>voice phrase already assigned to &quot;%1&quot;</source>
        <translation>이미 &quot;%1&quot;에 지정된 음성 문구입니다</translation>
    </message>
    <message>
        <location line="-72"/>
        <source>No snippet with that ID</source>
        <translation>해당 ID의 스니펫이 없습니다</translation>
    </message>
    <message>
        <location line="+15"/>
        <source>No such snippet</source>
        <translation>해당 스니펫이 없습니다</translation>
    </message>
    <message>
        <location line="+44"/>
        <source>Snippet limit reached (%1)</source>
        <translation>스니펫 개수 제한에 도달했습니다 (%1)</translation>
    </message>
    <message>
        <location line="+35"/>
        <source>Failed to save snippets on disk: %1</source>
        <translation>스니펫을 디스크에 저장하지 못했습니다: %1</translation>
    </message>
</context>
<context>
    <name>SnippetExtension</name>
    <message>
        <location filename="../src/builtins/snippet/snippet-extension.hpp" line="+10"/>
        <source>Snippets</source>
        <translation>스니펫</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Text expansion and snippet management</source>
        <translation>텍스트 확장 및 스니펫 관리</translation>
    </message>
</context>
<context>
    <name>SnippetFormView</name>
    <message>
        <location filename="../src/ui/qml/views/SnippetFormView.qml" line="+29"/>
        <source>Title</source>
        <translation>제목</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Euro symbol</source>
        <translation>유로 기호</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Content</source>
        <translation>내용</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>You can use {dynamic placeholders} to make the content dynamic: &lt;a href=&quot;https://docs.vicinae.com/snippets&quot;&gt;learn more&lt;/a&gt;.</source>
        <translation>{dynamic placeholders}로 동적 콘텐츠를 만들 수 있습니다: &lt;a href=&quot;https://docs.vicinae.com/snippets&quot;&gt;자세히 알아보기&lt;/a&gt;.</translation>
    </message>
    <message>
        <location line="+26"/>
        <source>Keyword</source>
        <translation>키워드</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Typing this keyword anywhere will result in it being replaced by the content of the snippet.</source>
        <translation>어디서든 이 키워드를 입력하면 스니펫의 내용으로 바뀝니다.</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>The snippet server is not running. Keyword expansion is unavailable. &lt;a href=&quot;https://docs.vicinae.com/snippets&quot;&gt;Learn more&lt;/a&gt;.</source>
        <translation>스니펫 서버가 실행 중이 아닙니다. 키워드 확장을 사용할 수 없습니다. &lt;a href=&quot;https://docs.vicinae.com/snippets&quot;&gt;자세히 알아보기&lt;/a&gt;.</translation>
    </message>
    <message>
        <location line="+13"/>
        <source>If a keyword is typed, it will only be expanded after space or punctuation.</source>
        <translation>키워드를 입력한 뒤 공백이나 문장 부호를 입력해야 확장됩니다.</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Expand as word</source>
        <translation>단어 단위로 확장</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Voice phrase</source>
        <translation>음성 문구</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Dictating exactly this phrase inserts the snippet instead.</source>
        <translation>이 문구를 정확히 말하면 대신 스니펫을 삽입합니다.</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>euro symbol</source>
        <translation>유로 기호</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Applications</source>
        <translation>앱</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Restrict the keyword and voice phrase to specific applications. By default, they work everywhere.</source>
        <translation>키워드와 음성 문구를 특정 앱에서만 사용하도록 제한합니다. 기본적으로 모든 앱에서 작동합니다.</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>+ Add app…</source>
        <translation>+ 앱 추가…</translation>
    </message>
</context>
<context>
    <name>SnippetFormViewHost</name>
    <message>
        <location filename="../src/builtins/snippet/snippet-form-view-host.cpp" line="+33"/>
        <source>Submit</source>
        <translation>제출</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Copy of %1</source>
        <translation>%1의 복사본</translation>
    </message>
    <message>
        <location line="+26"/>
        <source>Edit &quot;%1&quot;</source>
        <translation>&quot;%1&quot; 편집</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Duplicate &quot;%1&quot;</source>
        <translation>&quot;%1&quot; 복제</translation>
    </message>
    <message>
        <location line="+16"/>
        <source>2 chars min.</source>
        <translation>최소 2자를 입력하세요.</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Content should not be empty</source>
        <translation>내용은 비워 둘 수 없습니다</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Only one {cursor} placeholder is allowed</source>
        <translation>{cursor} 자리표시자는 하나만 사용할 수 있습니다</translation>
    </message>
    <message>
        <location line="+15"/>
        <source>Must contain a letter or a number</source>
        <translation>문자나 숫자를 포함해야 합니다</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Already used by &quot;%1&quot;</source>
        <translation>이미 &quot;%1&quot;에서 사용 중입니다</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Validation failed</source>
        <translation>유효성 검사 실패</translation>
    </message>
    <message>
        <location line="+30"/>
        <source>Snippet updated</source>
        <translation>스니펫을 수정했습니다</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Snippet successfully created</source>
        <translation>스니펫을 만들었습니다</translation>
    </message>
    <message>
        <location line="+12"/>
        <source>Cursor Position</source>
        <translation>커서 위치</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Clipboard Text</source>
        <translation>클립보드 텍스트</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Date</source>
        <translation>날짜</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Argument</source>
        <translation>인수</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>PowerShell Command</source>
        <translation>PowerShell 명령</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Shell Command</source>
        <translation>셸 명령</translation>
    </message>
</context>
<context>
    <name>SoftRebootCommand</name>
    <message>
        <location filename="../src/builtins/power-management/power-management-extension.cpp" line="-99"/>
        <source>Soft Reboot System</source>
        <translation>시스템 소프트 재부팅</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Soft reboot the system, which usually means only userspace is rebooted.</source>
        <translation>시스템을 소프트 재부팅합니다. 일반적으로 사용자 공간만 다시 시작합니다.</translation>
    </message>
    <message>
        <location line="+13"/>
        <source>System can&apos;t soft reboot</source>
        <translation>소프트 재부팅을 사용할 수 없습니다</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Failed to soft reboot</source>
        <translation>소프트 재부팅하지 못했습니다</translation>
    </message>
</context>
<context>
    <name>SponsorVicinaeCommand</name>
    <message>
        <location filename="../src/builtins/vicinae/vicinae-extension.cpp" line="-118"/>
        <source>Donate to Vicinae</source>
        <translation>Vicinae에 기부</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Open link to Vicinae&apos;s GitHub sponsor page</source>
        <translation>Vicinae의 GitHub 후원 페이지를 엽니다</translation>
    </message>
</context>
<context>
    <name>StoreDetailView</name>
    <message>
        <location filename="../src/ui/qml/views/StoreDetailView.qml" line="+208"/>
        <source>Installed</source>
        <translation>설치됨</translation>
    </message>
    <message>
        <location line="+171"/>
        <source>Description</source>
        <translation>설명</translation>
    </message>
    <message>
        <location line="+26"/>
        <source>Commands</source>
        <translation>명령</translation>
    </message>
    <message>
        <location line="+78"/>
        <source>Open README</source>
        <translation>README 열기</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Last update</source>
        <translation>마지막 업데이트</translation>
    </message>
    <message>
        <location line="+14"/>
        <source>Contributors</source>
        <translation>기여자</translation>
    </message>
    <message>
        <location line="+31"/>
        <source>Categories</source>
        <translation>카테고리</translation>
    </message>
    <message>
        <location line="+20"/>
        <source>Source Code</source>
        <translation>소스 코드</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>View Code</source>
        <translation>코드 보기</translation>
    </message>
</context>
<context>
    <name>SuspendCommand</name>
    <message>
        <location filename="../src/builtins/power-management/power-management-extension.cpp" line="+36"/>
        <source>Suspend System</source>
        <translation>시스템 대기 모드</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Suspend the system to RAM. Unlike hibernation, this does not turn the computer off and will break on power loss.</source>
        <translation>시스템 상태를 RAM에 유지하고 대기 모드로 전환합니다. 최대 절전 모드와 달리 컴퓨터 전원이 완전히 꺼지지 않으며 전원 공급이 끊기면 상태가 유실됩니다.</translation>
    </message>
    <message>
        <location line="+13"/>
        <source>System cannot suspend</source>
        <translation>대기 모드를 사용할 수 없습니다</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Failed to suspend</source>
        <translation>대기 모드로 전환하지 못했습니다</translation>
    </message>
</context>
<context>
    <name>SwitchWindowsCommand</name>
    <message>
        <location filename="../src/builtins/wm/wm-extension.cpp" line="+89"/>
        <source>Switch Windows</source>
        <translation>창 전환</translation>
    </message>
</context>
<context>
    <name>SwitchWindowsSection</name>
    <message>
        <location filename="../src/builtins/wm/switch-windows-model.cpp" line="+19"/>
        <source>WS %1</source>
        <translation>작업 공간 %1</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Window Actions</source>
        <translation>창 동작</translation>
    </message>
    <message>
        <location filename="../src/builtins/wm/switch-windows-model.hpp" line="+28"/>
        <source>Open Windows</source>
        <translation>열린 창</translation>
    </message>
</context>
<context>
    <name>SwitchWindowsViewHost</name>
    <message>
        <location filename="../src/builtins/wm/switch-windows-view-host.cpp" line="+12"/>
        <source>Search open window...</source>
        <translation>열린 창 검색...</translation>
    </message>
</context>
<context>
    <name>SwitchWorkspacesCommand</name>
    <message>
        <location filename="../src/builtins/wm/wm-extension.cpp" line="+11"/>
        <source>Switch Desktops</source>
        <translation>데스크톱 전환</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Switch Workspaces</source>
        <translation>작업 공간 전환</translation>
    </message>
</context>
<context>
    <name>SwitchWorkspacesSection</name>
    <message>
        <location filename="../src/builtins/wm/switch-workspaces-model.hpp" line="+47"/>
        <source>%1</source>
        <translation>%1</translation>
    </message>
    <message numerus="yes">
        <location line="+3"/>
        <source>%n window(s)</source>
        <translation>
            <numerusform>창 %n개</numerusform>
        </translation>
    </message>
    <message>
        <location line="+0"/>
        <source>empty</source>
        <translation>비어 있음</translation>
    </message>
    <message>
        <location line="+22"/>
        <source>Switch to desktop</source>
        <translation>데스크톱으로 전환</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Switch to workspace</source>
        <translation>작업 공간으로 전환</translation>
    </message>
</context>
<context>
    <name>SwitchWorkspacesViewHost</name>
    <message>
        <location filename="../src/builtins/wm/switch-workspaces-view-host.cpp" line="+13"/>
        <source>Search desktops...</source>
        <translation>데스크톱 검색...</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Search workspaces...</source>
        <translation>작업 공간 검색...</translation>
    </message>
    <message>
        <location filename="../src/builtins/wm/switch-workspaces-view-host.hpp" line="+16"/>
        <source>Open Workspaces</source>
        <translation>열린 작업 공간</translation>
    </message>
</context>
<context>
    <name>SystemBrowseApps</name>
    <message>
        <location filename="../src/builtins/system/system-extension.hpp" line="-17"/>
        <source>Browse Apps</source>
        <translation>앱 찾아보기</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Browse all applications that are installed on the system</source>
        <translation>시스템에 설치된 모든 앱을 살펴봅니다</translation>
    </message>
</context>
<context>
    <name>SystemExtension</name>
    <message>
        <location line="+39"/>
        <source>System</source>
        <translation>시스템</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>System-related commands</source>
        <translation>시스템 관련 명령</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>Bash</source>
        <translation>Bash</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Run shell commands on your computer.</source>
        <translation>컴퓨터에서 셸 명령을 실행합니다.</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Open</source>
        <translation>열기</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Open files, folders and links with their default application.</source>
        <translation>파일, 폴더, 링크를 기본 앱으로 엽니다.</translation>
    </message>
</context>
<context>
    <name>SystemRunCommand</name>
    <message>
        <location line="-126"/>
        <source>Run Terminal Program</source>
        <translation>터미널 프로그램 실행</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Run a program in a terminal window</source>
        <translation>터미널 창에서 프로그램을 실행합니다</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>command</source>
        <translation>명령</translation>
    </message>
    <message>
        <location line="+26"/>
        <source>Not a valid executable</source>
        <translation>올바른 실행 파일이 아닙니다</translation>
    </message>
</context>
<context>
    <name>SystemRunViewHost</name>
    <message>
        <location filename="../src/builtins/system/system-run-view-host.cpp" line="+19"/>
        <source>Search for a program to execute...</source>
        <translation>실행할 프로그램 검색...</translation>
    </message>
</context>
<context>
    <name>ThemeExtension</name>
    <message>
        <location filename="../src/builtins/theme/theme-extension.hpp" line="+9"/>
        <source>Theme</source>
        <translation>테마</translation>
    </message>
</context>
<context>
    <name>ThemeSection</name>
    <message>
        <location filename="../src/builtins/theme/theme-list-model.cpp" line="+22"/>
        <source>Default theme description</source>
        <translation>기본 테마 설명</translation>
    </message>
    <message>
        <location line="+64"/>
        <source>Open theme file</source>
        <translation>테마 파일 열기</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Copy ID</source>
        <translation>ID 복사</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Copy path</source>
        <translation>경로 복사</translation>
    </message>
</context>
<context>
    <name>ThemeViewHost</name>
    <message>
        <location filename="../src/builtins/theme/theme-view-host.cpp" line="+22"/>
        <source>Search for a theme...</source>
        <translation>테마 검색...</translation>
    </message>
    <message>
        <location line="+53"/>
        <source>Current Theme</source>
        <translation>현재 테마</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Available Themes</source>
        <translation>사용 가능한 테마</translation>
    </message>
</context>
<context>
    <name>ToggleFloatingWindowCommand</name>
    <message>
        <location filename="../src/builtins/wm/wm-extension.cpp" line="-54"/>
        <source>Toggle Floating</source>
        <translation>플로팅 전환</translation>
    </message>
    <message>
        <location line="+13"/>
        <source>Active window is not on the current workspace</source>
        <translation>활성 창이 현재 작업 공간에 없습니다</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>No window to toggle</source>
        <translation>전환할 창이 없습니다</translation>
    </message>
</context>
<context>
    <name>ToggleFullscreenWindowCommand</name>
    <message>
        <location line="-44"/>
        <source>Toggle Fullscreen</source>
        <translation>전체 화면 전환</translation>
    </message>
    <message>
        <location line="+13"/>
        <source>Active window is not on the current workspace</source>
        <translation>활성 창이 현재 작업 공간에 없습니다</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>No window to fullscreen</source>
        <translation>전체 화면으로 전환할 창이 없습니다</translation>
    </message>
</context>
<context>
    <name>ToggleItemAsFavorite</name>
    <message>
        <location filename="../src/actions/root-search-actions.cpp" line="-86"/>
        <source>Remove from favorites</source>
        <translation>즐겨찾기에서 제거</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Add to favorites</source>
        <translation>즐겨찾기에 추가</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Successfuly added to favorites</source>
        <translation>즐겨찾기에 추가했습니다</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Successfuly removed from favorites</source>
        <translation>즐겨찾기에서 제거했습니다</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Failed to add to favorites</source>
        <translation>즐겨찾기에 추가하지 못했습니다</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Failed to remove from favorites</source>
        <translation>즐겨찾기에서 제거하지 못했습니다</translation>
    </message>
</context>
<context>
    <name>ToggleMuteCommand</name>
    <message>
        <location filename="../src/builtins/media/media-extension.hpp" line="+11"/>
        <source>Toggle Mute</source>
        <translation>음소거 전환</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Mute or unmute system audio</source>
        <translation>시스템 오디오를 음소거하거나 해제합니다</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Failed to toggle mute</source>
        <translation>음소거 상태를 전환하지 못했습니다</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Muted</source>
        <translation>음소거됨</translation>
    </message>
</context>
<context>
    <name>ToggleOverviewCommand</name>
    <message>
        <location filename="../src/builtins/wm/wm-extension.cpp" line="+34"/>
        <source>Toggle Overview</source>
        <translation>개요 화면 전환</translation>
    </message>
</context>
<context>
    <name>ToolApprovalPanel</name>
    <message>
        <location filename="../src/ui/qml/controls/ToolApprovalPanel.qml" line="+28"/>
        <source>Allow %1 to run?</source>
        <translation>%1 실행을 허용할까요?</translation>
    </message>
    <message>
        <location line="+63"/>
        <source>Deny</source>
        <translation>거부</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Always Allow</source>
        <translation>항상 허용</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Allow Once</source>
        <translation>한 번만 허용</translation>
    </message>
</context>
<context>
    <name>ToolInvocation</name>
    <message>
        <location filename="../src/ui/qml/controls/ToolInvocation.qml" line="+59"/>
        <source>Show Arguments</source>
        <translation>인수 표시</translation>
    </message>
    <message>
        <location line="+66"/>
        <source>No output</source>
        <translation>출력 없음</translation>
    </message>
</context>
<context>
    <name>ToolInvocationHeader</name>
    <message>
        <location filename="../src/ui/qml/controls/ToolInvocationHeader.qml" line="+24"/>
        <source>Running…</source>
        <translation>실행 중…</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Waiting for approval</source>
        <translation>승인 대기 중</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Declined</source>
        <translation>거부됨</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Completed</source>
        <translation>완료됨</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Failed</source>
        <translation>실패</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Cancelled</source>
        <translation>취소됨</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Queued</source>
        <translation>대기 중</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>%1 · %2</source>
        <translation>%1 · %2</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>%1 — %2 — %3</source>
        <translation>%1 — %2 — %3</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>%1 — %2</source>
        <translation>%1 — %2</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Collapse tool details</source>
        <translation>도구 세부 정보 접기</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Expand tool details</source>
        <translation>도구 세부 정보 펼치기</translation>
    </message>
</context>
<context>
    <name>TranscribeCommand</name>
    <message>
        <location filename="../src/builtins/dictation/dictation-command.cpp" line="+46"/>
        <source>Could not start recording</source>
        <translation>녹음을 시작할 수 없습니다</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>Could not paste, copied to clipboard</source>
        <translation>붙여넣을 수 없어 클립보드에 복사했습니다</translation>
    </message>
    <message>
        <location line="+25"/>
        <source>Allow microphone access</source>
        <translation>마이크 접근 허용</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Dictation needs to hear you. Turn on the microphone for Vicinae in System Settings, then try again.</source>
        <translation>음성 입력에는 마이크가 필요합니다. 시스템 설정에서 Vicinae의 마이크 권한을 켠 후 다시 시도하세요.</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Open System Settings</source>
        <translation>시스템 설정 열기</translation>
    </message>
    <message>
        <location line="+37"/>
        <source>Set up dictation</source>
        <translation>음성 입력 설정</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Dictation needs a speech model before it can turn your voice into text. Install one that runs on this machine, or add an AI provider that offers transcription.</source>
        <translation>음성을 텍스트로 변환하려면 음성 모델이 필요합니다. 이 컴퓨터에서 실행할 모델을 설치하거나 음성 인식을 제공하는 AI 제공자를 추가하세요.</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Open AI Settings</source>
        <translation>AI 설정 열기</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Choose a transcription model</source>
        <translation>음성 인식 모델 선택</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Pick the model dictation should use from the dictation settings. You can change it at any time.</source>
        <translation>음성 입력 설정에서 사용할 모델을 선택하세요. 언제든 변경할 수 있습니다.</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Open Dictation Settings</source>
        <translation>음성 입력 설정 열기</translation>
    </message>
    <message>
        <location filename="../src/builtins/dictation/dictation-command.hpp" line="+14"/>
        <source>Dictate</source>
        <translation>음성으로 입력</translation>
    </message>
</context>
<context>
    <name>TranscribeViewHost</name>
    <message>
        <location filename="../src/builtins/dictation/transcribe-view-host.cpp" line="+21"/>
        <location line="+40"/>
        <source>Recording</source>
        <translation>녹음 중</translation>
    </message>
    <message>
        <location line="-4"/>
        <source>Rewriting</source>
        <translation>다시 쓰는 중</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Transcribing</source>
        <translation>텍스트로 변환 중</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Stop &amp; Transcribe</source>
        <translation>중지 및 텍스트 변환</translation>
    </message>
</context>
<context>
    <name>TranscriptionSession</name>
    <message>
        <location filename="../src/services/dictation/transcription-session.cpp" line="+46"/>
        <source>Microphone stopped</source>
        <translation>마이크가 중지되었습니다</translation>
    </message>
    <message>
        <location line="+12"/>
        <source>Could not start recording</source>
        <translation>녹음을 시작할 수 없습니다</translation>
    </message>
    <message>
        <location line="+68"/>
        <location line="+31"/>
        <source>Nothing to transcribe</source>
        <translation>텍스트로 변환할 내용이 없습니다</translation>
    </message>
    <message>
        <location line="-5"/>
        <source>Transcription failed</source>
        <translation>음성 인식 실패</translation>
    </message>
</context>
<context>
    <name>TrayMenuViewHost</name>
    <message>
        <location filename="../src/builtins/vicinae/search-tray-view-host.hpp" line="-122"/>
        <source>Search menu...</source>
        <translation>메뉴 검색...</translation>
    </message>
    <message>
        <location line="+36"/>
        <source>Trigger</source>
        <translation>실행</translation>
    </message>
</context>
<context>
    <name>TrayService</name>
    <message>
        <location filename="../src/services/tray/tray-service.cpp" line="+10"/>
        <source>Toggle Vicinae</source>
        <translation>Vicinae 표시 전환</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>About Vicinae</source>
        <translation>Vicinae 정보</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Check for Updates…</source>
        <translation>업데이트 확인…</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Update Available: %1</source>
        <translation>업데이트 사용 가능: %1</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Settings…</source>
        <translation>설정…</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Preferences…</source>
        <translation>환경설정…</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Sponsor Vicinae</source>
        <translation>Vicinae 후원</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Join the Discord</source>
        <translation>Discord 참여</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Follow on X</source>
        <translation>X에서 팔로우</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Quit Vicinae</source>
        <translation>Vicinae 종료</translation>
    </message>
</context>
<context>
    <name>UninstallAppAction</name>
    <message>
        <location filename="../src/actions/app-actions.cpp" line="+20"/>
        <source>Uninstall Application</source>
        <translation>앱 제거</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Failed to uninstall %1</source>
        <translation>%1을 제거하지 못했습니다</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Uninstalled %1</source>
        <translation>%1을 제거했습니다</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Failed to quit %1</source>
        <translation>%1을 종료하지 못했습니다</translation>
    </message>
    <message>
        <location line="+15"/>
        <source>%1 did not quit, uninstall cancelled</source>
        <translation>%1이 종료되지 않아 제거를 취소했습니다</translation>
    </message>
    <message>
        <location line="+12"/>
        <source>%1 is running. It will be quit and moved to the trash.</source>
        <translation>%1이 실행 중입니다. 앱을 종료한 후 휴지통으로 이동합니다.</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>The application will be moved to the trash.</source>
        <translation>앱을 휴지통으로 이동합니다.</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Uninstall %1?</source>
        <translation>%1을 제거할까요?</translation>
    </message>
</context>
<context>
    <name>UninstallExtensionAction</name>
    <message>
        <location filename="../src/actions/extension-actions.cpp" line="+10"/>
        <source>Are you sure?</source>
        <translation>계속할까요?</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>All this extension data will be permanently lost. If you just want the extension to not appear in the root search anymore, consider disabling it instead.</source>
        <translation>이 확장 프로그램의 모든 데이터가 영구적으로 삭제됩니다. 메인 화면 검색에서 확장 프로그램을 숨기려는 경우 비활성화를 고려하세요.</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Uninstall</source>
        <translation>제거</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Extension uninstalled</source>
        <translation>확장 프로그램을 제거했습니다</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Failed to uninstall extension</source>
        <translation>확장 프로그램을 제거하지 못했습니다</translation>
    </message>
    <message>
        <location filename="../src/actions/extension-actions.hpp" line="+10"/>
        <source>Uninstall Extension</source>
        <translation>확장 프로그램 제거</translation>
    </message>
</context>
<context>
    <name>UnpinCalculatorHistoryRecordAction</name>
    <message>
        <location filename="../src/actions/calculator-actions.hpp" line="-23"/>
        <source>Entry unpinned</source>
        <translation>항목의 고정을 해제했습니다</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Unpin entry</source>
        <translation>항목 고정 해제</translation>
    </message>
</context>
<context>
    <name>UnpinEmojiAction</name>
    <message>
        <location filename="../src/builtins/vicinae/emoji-grid-model.cpp" line="-45"/>
        <source>Unpin emoji</source>
        <translation>이모지 고정 해제</translation>
    </message>
</context>
<context>
    <name>UpdateService</name>
    <message>
        <location filename="../src/services/update/update-service.cpp" line="-169"/>
        <source>Update installed</source>
        <translation>업데이트를 설치했습니다</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Restarting…</source>
        <translation>다시 시작 중…</translation>
    </message>
    <message>
        <location line="+90"/>
        <source>Downloading Vicinae %1…</source>
        <translation>Vicinae %1 다운로드 중…</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Downloading Vicinae %1… %2%</source>
        <translation>Vicinae %1 다운로드 중… %2%</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Installing update…</source>
        <translation>업데이트 설치 중…</translation>
    </message>
    <message>
        <location line="+24"/>
        <source>Update failed</source>
        <translation>업데이트 실패</translation>
    </message>
</context>
<context>
    <name>VicinaeExtension</name>
    <message>
        <location filename="../src/builtins/vicinae/vicinae-extension.cpp" line="+227"/>
        <source>Read Configuration</source>
        <translation>설정 읽기</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Read the default and current Vicinae configuration.</source>
        <translation>Vicinae의 기본 설정과 현재 설정을 읽습니다.</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Change Configuration</source>
        <translation>설정 변경</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Change your Vicinae configuration.</source>
        <translation>Vicinae 설정을 변경합니다.</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Read Themes</source>
        <translation>테마 읽기</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>List installed themes and read the theme file format.</source>
        <translation>설치된 테마 목록과 테마 파일 형식을 확인합니다.</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Write Theme</source>
        <translation>테마 작성</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Create your own themes. Asks before replacing an existing one.</source>
        <translation>나만의 테마를 만듭니다. 기존 테마를 덮어쓰기 전에 확인합니다.</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Set Theme</source>
        <translation>테마 설정</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Switch the active theme.</source>
        <translation>사용 중인 테마를 전환합니다.</translation>
    </message>
    <message>
        <location filename="../src/builtins/vicinae/vicinae-extension.hpp" line="+11"/>
        <source>General vicinae-related commands.</source>
        <translation>Vicinae 관련 일반 명령입니다.</translation>
    </message>
</context>
<context>
    <name>VicinaeHotkeyGlobalShortcutBackend</name>
    <message>
        <location filename="../src/services/global-shortcuts/vicinae-hotkey-global-shortcut-backend.cpp" line="+79"/>
        <source>Compositor does not support global hotkeys</source>
        <translation>컴포지터에서 전역 단축키를 지원하지 않습니다</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Unsupported trigger key</source>
        <translation>지원하지 않는 트리거 키</translation>
    </message>
    <message>
        <location line="+17"/>
        <source>Hotkey binding was lost</source>
        <translation>단축키 연결이 해제되었습니다</translation>
    </message>
</context>
<context>
    <name>VicinaeListInstalledExtensionsCommand</name>
    <message>
        <location filename="../src/builtins/vicinae/list-installed-extensions-command.hpp" line="+11"/>
        <source>Show Installed Extensions</source>
        <translation>설치된 확장 프로그램 표시</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Show all third-party extensions that have been installed. This includes local extensions as well as extensions downloaded from the stores (vicinae and raycast).</source>
        <translation>설치된 모든 타사 확장 프로그램을 표시합니다. 로컬 확장 프로그램과 스토어(Vicinae 및 Raycast)에서 다운로드한 확장 프로그램이 포함됩니다.</translation>
    </message>
</context>
<context>
    <name>VicinaeStoreCommand</name>
    <message>
        <location filename="../src/builtins/vicinae/vicinae-store-command.hpp" line="+15"/>
        <source>Install extensions from the Vicinae store</source>
        <translation>Vicinae store에서 확장 프로그램 설치</translation>
    </message>
    <message>
        <location line="+13"/>
        <source>
# Welcome to the vicinae extension store

The vicinae extension store features community-built extensions that have been approved by our core contributors.

Every extension listed here has its source code available in the [vicinaehq/extensions](https://github.com/vicinaehq/extensions) repository.

If you&apos;re looking to build your own extension, take a look at the [documentation](https://docs.vicinae.com/extensions/introduction). If you think your extension would be a good fit for the store, feel free to submit it!
</source>
        <translation>
# Vicinae Extension Store에 오신 것을 환영합니다

Vicinae Extension Store는 핵심 기여자가 승인한 커뮤니티 제작 확장 프로그램을 제공합니다.

여기 있는 모든 확장 프로그램의 소스 코드는 [vicinaehq/extensions](https://github.com/vicinaehq/extensions) 저장소에서 확인할 수 있습니다.

직접 확장 프로그램을 만들려면 [문서](https://docs.vicinae.com/extensions/introduction)를 살펴보세요. 스토어에 어울리는 확장 프로그램이라면 언제든 제출해 주세요!
</translation>
    </message>
    <message>
        <location line="+13"/>
        <source>Continue to store</source>
        <translation>스토어로 이동</translation>
    </message>
</context>
<context>
    <name>VicinaeStoreDetailHost</name>
    <message>
        <location filename="../src/builtins/vicinae/vicinae-store-detail-host.cpp" line="+36"/>
        <source>Failed to load extension</source>
        <translation>확장 프로그램을 불러오지 못했습니다</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Could not fetch extension data from the store.</source>
        <translation>스토어에서 확장 프로그램 데이터를 가져올 수 없습니다.</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>Extension not found</source>
        <translation>확장 프로그램을 찾을 수 없음</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>The extension &quot;%1&quot; could not be found in the store.</source>
        <translation>스토어에서 확장 프로그램 &quot;%1&quot;을 찾을 수 없습니다.</translation>
    </message>
    <message>
        <location line="+20"/>
        <source>Extension Store - %1</source>
        <translation>Extension Store - %1</translation>
    </message>
    <message>
        <location line="+73"/>
        <source>Extension Store</source>
        <translation>Extension Store</translation>
    </message>
    <message>
        <location line="+17"/>
        <source>Report issue</source>
        <translation>문제 신고</translation>
    </message>
</context>
<context>
    <name>VicinaeStoreSection</name>
    <message>
        <location filename="../src/builtins/vicinae/vicinae-store-model.cpp" line="+42"/>
        <source>Show details</source>
        <translation>세부 정보 표시</translation>
    </message>
</context>
<context>
    <name>VicinaeStoreViewHost</name>
    <message>
        <location filename="../src/builtins/vicinae/vicinae-store-view-host.cpp" line="+26"/>
        <source>Browse Vicinae extensions</source>
        <translation>Vicinae 확장 프로그램 찾아보기</translation>
    </message>
    <message>
        <location line="+23"/>
        <source>Failed to fetch extensions</source>
        <translation>확장 프로그램을 가져오지 못했습니다</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Extensions</source>
        <translation>확장 프로그램</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Extension Store</source>
        <translation>Extension Store</translation>
    </message>
</context>
<context>
    <name>VocabularyCommand</name>
    <message>
        <location filename="../src/builtins/dictation/dictation-extension.hpp" line="-11"/>
        <source>Vocabulary</source>
        <translation>단어장</translation>
    </message>
</context>
<context>
    <name>VocabularyFormView</name>
    <message>
        <location filename="../src/ui/qml/views/VocabularyFormView.qml" line="+16"/>
        <source>Word</source>
        <translation>단어</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>A name, term or acronym you dictate often and want transcribed exactly this way.</source>
        <translation>자주 말하며 정확히 이 표기로 인식되기를 원하는 이름, 용어, 약어입니다.</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Vicinae</source>
        <translation>Vicinae</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Hint</source>
        <translation>힌트</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Optional. Helps rewrite styles understand this word, such as who a name refers to. Not used for transcription.</source>
        <translation>선택 사항입니다. 이름이 누구를 가리키는지 등 다시 쓰기 스타일이 이 단어를 이해하는 데 도움을 줍니다. 음성 인식에는 사용하지 않습니다.</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>A launcher app, often misheard as &quot;Vicinity&quot;</source>
        <translation>&quot;Vicinity&quot;로 잘못 인식되는 경우가 많은 런처 앱</translation>
    </message>
</context>
<context>
    <name>VocabularyFormViewHost</name>
    <message>
        <location filename="../src/builtins/dictation/vocabulary-form-view-host.cpp" line="+26"/>
        <source>Edit Word</source>
        <translation>단어 편집</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Add Word to Vocabulary</source>
        <translation>단어장에 단어 추가</translation>
    </message>
    <message>
        <location line="+12"/>
        <source>Save Word</source>
        <translation>단어 저장</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Add Word</source>
        <translation>단어 추가</translation>
    </message>
    <message>
        <location line="+16"/>
        <source>Enter a word</source>
        <translation>단어를 입력하세요</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Already in your vocabulary</source>
        <translation>이미 단어장에 있습니다</translation>
    </message>
    <message numerus="yes">
        <location line="+3"/>
        <source>Keep the hint under %n character(s)</source>
        <translation>
            <numerusform>힌트는 %n자 미만으로 입력하세요</numerusform>
        </translation>
    </message>
    <message>
        <location line="+11"/>
        <source>Failed to save word</source>
        <translation>단어를 저장하지 못했습니다</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Failed to add word</source>
        <translation>단어를 추가하지 못했습니다</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Word saved</source>
        <translation>단어를 저장했습니다</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Added to vocabulary</source>
        <translation>단어장에 추가했습니다</translation>
    </message>
</context>
<context>
    <name>VocabularyViewHost</name>
    <message>
        <location filename="../src/builtins/dictation/vocabulary-view-host.hpp" line="+36"/>
        <source>Search vocabulary...</source>
        <translation>단어장 검색...</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>No words yet</source>
        <translation>아직 단어가 없습니다</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Words you add here help dictation spell them right.</source>
        <translation>여기에 단어를 추가하면 음성 입력 시 정확한 표기로 인식하는 데 도움이 됩니다.</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Words ({count})</source>
        <translation>단어 ({count})</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Added %1</source>
        <translation>추가: %1</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Copy</source>
        <translation>복사</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Edit Word</source>
        <translation>단어 편집</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Remove Word</source>
        <translation>단어 삭제</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Failed to remove word</source>
        <translation>단어를 삭제하지 못했습니다</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Remove All Words</source>
        <translation>모든 단어 삭제</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Remove all words?</source>
        <translation>모든 단어를 삭제할까요?</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Your whole vocabulary will be lost forever.</source>
        <translation>단어장 전체가 영구적으로 삭제됩니다.</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Remove all</source>
        <translation>모두 삭제</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Vocabulary cleared</source>
        <translation>단어장을 지웠습니다</translation>
    </message>
    <message>
        <location line="+27"/>
        <source>Add Word</source>
        <translation>단어 추가</translation>
    </message>
</context>
<context>
    <name>VolumeDownCommand</name>
    <message>
        <location filename="../src/builtins/media/media-extension.hpp" line="-74"/>
        <source>Turn Volume Down</source>
        <translation>음량 줄이기</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Decrease system volume</source>
        <translation>시스템 음량 줄이기</translation>
    </message>
    <message>
        <location line="+18"/>
        <source>Invalid step value</source>
        <translation>잘못된 조절 단위</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Failed to adjust volume</source>
        <translation>음량을 조절하지 못했습니다</translation>
    </message>
</context>
<context>
    <name>VolumeUpCommand</name>
    <message>
        <location line="-61"/>
        <source>Turn Volume Up</source>
        <translation>음량 높이기</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Increase system volume</source>
        <translation>시스템 음량 높이기</translation>
    </message>
    <message>
        <location line="+18"/>
        <source>Invalid step value</source>
        <translation>잘못된 조절 단위</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Failed to adjust volume</source>
        <translation>음량을 조절하지 못했습니다</translation>
    </message>
</context>
<context>
    <name>WallpaperManager</name>
    <message>
        <location filename="../src/services/wallpaper/wallpaper-manager.cpp" line="+68"/>
        <source>Setting the wallpaper is not supported in the current environment</source>
        <translation>현재 환경에서는 배경화면 설정을 지원하지 않습니다</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>No such file: %1</source>
        <translation>파일이 없습니다: %1</translation>
    </message>
</context>
<context>
    <name>WinControlPanelRootItem</name>
    <message>
        <location filename="../src/root-search/control-panel/control-panel-root-provider.cpp" line="+104"/>
        <location line="+10"/>
        <source>Control Panel</source>
        <translation>제어판</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Name</source>
        <translation>이름</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Where</source>
        <translation>위치</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>Open Applet</source>
        <translation>제어판 항목 열기</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Copy Path</source>
        <translation>경로 복사</translation>
    </message>
</context>
<context>
    <name>WinControlPanelRootProvider</name>
    <message>
        <location line="+51"/>
        <source>Control Panel</source>
        <translation>제어판</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Control Panel applets and system tasks.</source>
        <translation>제어판 항목 및 시스템 작업입니다.</translation>
    </message>
</context>
<context>
    <name>WinControlPanelTaskRootItem</name>
    <message>
        <location line="-41"/>
        <location line="+11"/>
        <source>Control Panel</source>
        <translation>제어판</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Name</source>
        <translation>이름</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Task ID</source>
        <translation>작업 ID</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Open</source>
        <translation>열기</translation>
    </message>
</context>
<context>
    <name>WinSettingsPage</name>
    <message>
        <location filename="../src/root-search/windows-settings/windows-settings-root-provider.cpp" line="-114"/>
        <source>Display</source>
        <translation>디스플레이</translation>
    </message>
    <message>
        <location line="+0"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <source>System</source>
        <translation>시스템</translation>
    </message>
    <message>
        <location line="-17"/>
        <source>Night Light</source>
        <translation>야간 모드</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Sound</source>
        <translation>소리</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Volume Mixer</source>
        <translation>볼륨 믹서</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Notifications</source>
        <translation>알림</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Focus</source>
        <translation>집중</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Power &amp; Battery</source>
        <translation>전원 및 배터리</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Storage</source>
        <translation>저장소</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Nearby Sharing</source>
        <translation>근거리 공유</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Multitasking</source>
        <translation>멀티태스킹</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Activation</source>
        <translation>정품 인증</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Troubleshoot</source>
        <translation>문제 해결</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Recovery</source>
        <translation>복구</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Projecting to This PC</source>
        <translation>이 PC에 화면 표시</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Remote Desktop</source>
        <translation>원격 데스크톱</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Clipboard</source>
        <translation>클립보드</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>About</source>
        <translation>정보</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Optional Features</source>
        <translation>선택적 기능</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>For Developers</source>
        <translation>개발자용</translation>
    </message>
    <message>
        <location line="+2"/>
        <location line="+0"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <source>Bluetooth &amp; Devices</source>
        <translation>Bluetooth 및 장치</translation>
    </message>
    <message>
        <location line="-8"/>
        <source>Devices</source>
        <translation>장치</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Printers &amp; Scanners</source>
        <translation>프린터 및 스캐너</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Mobile Devices</source>
        <translation>모바일 장치</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Cameras</source>
        <translation>카메라</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Mouse</source>
        <translation>마우스</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Touchpad</source>
        <translation>터치패드</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Pen &amp; Windows Ink</source>
        <translation>펜 및 Windows Ink</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>AutoPlay</source>
        <translation>자동 실행</translation>
    </message>
    <message>
        <location line="+3"/>
        <location line="+0"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <source>Network &amp; Internet</source>
        <translation>네트워크 및 인터넷</translation>
    </message>
    <message>
        <location line="-7"/>
        <source>Wi-Fi</source>
        <translation>Wi-Fi</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Ethernet</source>
        <translation>이더넷</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Mobile Hotspot</source>
        <translation>모바일 핫스팟</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Airplane Mode</source>
        <translation>비행기 모드</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Proxy</source>
        <translation>프록시</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Dial-up</source>
        <translation>전화 접속</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Advanced Network Settings</source>
        <translation>고급 네트워크 설정</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Background</source>
        <translation>배경</translation>
    </message>
    <message>
        <location line="+0"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <source>Personalization</source>
        <translation>개인 설정</translation>
    </message>
    <message>
        <location line="-7"/>
        <source>Colors</source>
        <translation>색</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Themes</source>
        <translation>테마</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Lock Screen</source>
        <translation>잠금 화면</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Touch Keyboard</source>
        <translation>터치 키보드</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Start</source>
        <translation>시작</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Taskbar</source>
        <translation>작업 표시줄</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Fonts</source>
        <translation>글꼴</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Dynamic Lighting</source>
        <translation>동적 조명</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Installed Apps</source>
        <translation>설치된 앱</translation>
    </message>
    <message>
        <location line="+0"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <source>Apps</source>
        <translation>앱</translation>
    </message>
    <message>
        <location line="-4"/>
        <source>Default Apps</source>
        <translation>기본 앱</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Offline Maps</source>
        <translation>오프라인 지도</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Apps for Websites</source>
        <translation>웹 사이트용 앱</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Video Playback</source>
        <translation>비디오 재생</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Startup Apps</source>
        <translation>시작 프로그램</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Your Info</source>
        <translation>사용자 정보</translation>
    </message>
    <message>
        <location line="+0"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <source>Accounts</source>
        <translation>계정</translation>
    </message>
    <message>
        <location line="-4"/>
        <source>Email &amp; Accounts</source>
        <translation>이메일 및 계정</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Sign-in Options</source>
        <translation>로그인 옵션</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Access Work or School</source>
        <translation>회사 또는 학교 액세스</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Family &amp; Other Users</source>
        <translation>가족 및 다른 사용자</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Windows Backup</source>
        <translation>Windows 백업</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Date &amp; Time</source>
        <translation>날짜 및 시간</translation>
    </message>
    <message>
        <location line="+0"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <source>Time &amp; Language</source>
        <translation>시간 및 언어</translation>
    </message>
    <message>
        <location line="-2"/>
        <source>Language &amp; Region</source>
        <translation>언어 및 지역</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Typing</source>
        <translation>입력</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Speech</source>
        <translation>음성</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Game Bar</source>
        <translation>Game Bar</translation>
    </message>
    <message>
        <location line="+0"/>
        <location line="+1"/>
        <location line="+1"/>
        <source>Gaming</source>
        <translation>게임</translation>
    </message>
    <message>
        <location line="-1"/>
        <source>Captures</source>
        <translation>캡처</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Game Mode</source>
        <translation>게임 모드</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Text Size</source>
        <translation>텍스트 크기</translation>
    </message>
    <message>
        <location line="+0"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <source>Accessibility</source>
        <translation>접근성</translation>
    </message>
    <message>
        <location line="-9"/>
        <source>Visual Effects</source>
        <translation>시각 효과</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Magnifier</source>
        <translation>돋보기</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Color Filters</source>
        <translation>색상 필터</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Contrast Themes</source>
        <translation>대비 테마</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Narrator</source>
        <translation>내레이터</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Accessibility Audio</source>
        <translation>접근성 오디오</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Captions</source>
        <translation>자막</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Accessibility Keyboard</source>
        <translation>접근성 키보드</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Accessibility Mouse</source>
        <translation>접근성 마우스</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Eye Control</source>
        <translation>아이 컨트롤</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Windows Security</source>
        <translation>Windows 보안</translation>
    </message>
    <message>
        <location line="+0"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <source>Privacy &amp; Security</source>
        <translation>개인 정보 및 보안</translation>
    </message>
    <message>
        <location line="-7"/>
        <source>Find My Device</source>
        <translation>내 장치 찾기</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Privacy</source>
        <translation>개인정보 보호</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Location</source>
        <translation>위치</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Camera Access</source>
        <translation>카메라 액세스</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Microphone Access</source>
        <translation>마이크 액세스</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Activity History</source>
        <translation>활동 기록</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Diagnostics &amp; Feedback</source>
        <translation>진단 및 피드백</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Search Permissions</source>
        <translation>검색 권한</translation>
    </message>
    <message>
        <location line="+2"/>
        <location line="+0"/>
        <location line="+1"/>
        <location line="+1"/>
        <location line="+1"/>
        <source>Windows Update</source>
        <translation>Windows 업데이트</translation>
    </message>
    <message>
        <location line="-2"/>
        <source>Update History</source>
        <translation>업데이트 기록</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Advanced Update Options</source>
        <translation>고급 업데이트 옵션</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Windows Insider Program</source>
        <translation>Windows 참가자 프로그램</translation>
    </message>
</context>
<context>
    <name>WinSettingsPageRootItem</name>
    <message>
        <location line="+37"/>
        <source>System Settings</source>
        <translation>시스템 설정</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Settings</source>
        <translation>설정</translation>
    </message>
    <message>
        <location line="+12"/>
        <source>Name</source>
        <translation>이름</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Category</source>
        <translation>카테고리</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Open %1 Settings</source>
        <translation>%1 설정 열기</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Copy URL</source>
        <translation>URL 복사</translation>
    </message>
</context>
<context>
    <name>WinSettingsRootProvider</name>
    <message>
        <location line="+13"/>
        <source>Windows Settings</source>
        <translation>Windows 설정</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Pages of the Windows Settings app.</source>
        <translation>Windows 설정 앱의 페이지입니다.</translation>
    </message>
</context>
<context>
    <name>WindowManagementExtension</name>
    <message>
        <location filename="../src/builtins/wm/wm-extension.cpp" line="+56"/>
        <source>Window Management</source>
        <translation>창 관리</translation>
    </message>
</context>
<context>
    <name>WindowsAppDatabase</name>
    <message>
        <location filename="../src/services/app-service/windows/win-app-database.cpp" line="+1125"/>
        <source>File Explorer</source>
        <translation>파일 탐색기</translation>
    </message>
    <message>
        <location line="+20"/>
        <source>Terminal</source>
        <translation>터미널</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Command Prompt</source>
        <translation>명령 프롬프트</translation>
    </message>
</context>
<context>
    <name>WindowsApplication</name>
    <message>
        <location filename="../src/services/app-service/windows/win-app.hpp" line="+74"/>
        <source>%1: Run as Administrator</source>
        <translation>%1: 관리자 권한으로 실행</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Run as Administrator</source>
        <translation>관리자 권한으로 실행</translation>
    </message>
</context>
<context>
    <name>WindowsGlobalShortcutBackend</name>
    <message>
        <location filename="../src/services/global-shortcuts/windows-global-shortcut-backend.cpp" line="+302"/>
        <source>unsupported or invalid trigger</source>
        <translation>지원하지 않거나 잘못된 트리거</translation>
    </message>
</context>
<context>
    <name>WindowsUpdateInstaller</name>
    <message>
        <location filename="../src/services/update/windows-update-installer.cpp" line="+184"/>
        <source>This installation cannot update itself</source>
        <translation>이 설치본은 자체 업데이트할 수 없습니다</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>The update is not signed</source>
        <translation>업데이트가 서명되지 않았습니다</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Update signature verification failed (0x%1)</source>
        <translation>업데이트 서명을 확인하지 못했습니다 (0x%1)</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Update is signed by %1, expected %2</source>
        <translation>업데이트 서명자가 예상과 다릅니다 (예상: %2, 실제: %1)</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Verifying update…</source>
        <translation>업데이트 확인 중…</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>Update has no version information</source>
        <translation>업데이트에 버전 정보가 없습니다</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Update version mismatch: expected %1, found %2</source>
        <translation>업데이트 버전 불일치: 예상 %1, 실제 %2</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Starting installer…</source>
        <translation>설치 프로그램 시작 중…</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Failed to start the installer</source>
        <translation>설치 프로그램을 시작하지 못했습니다</translation>
    </message>
</context>
<context>
    <name>X11GlobalShortcutBackend</name>
    <message>
        <location filename="../src/services/global-shortcuts/x11-global-shortcut-backend.cpp" line="+147"/>
        <source>This shortcut is already in use by another application</source>
        <translation>다른 앱에서 이미 사용 중인 단축키입니다</translation>
    </message>
    <message>
        <location line="+15"/>
        <source>Modifier-only shortcuts are not supported</source>
        <translation>보조 키만 사용하는 단축키는 지원하지 않습니다</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Unsupported trigger key</source>
        <translation>지원하지 않는 트리거 키</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Trigger key is not present on this keyboard</source>
        <translation>이 키보드에 트리거 키가 없습니다</translation>
    </message>
</context>
<context>
    <name>X11Workspace</name>
    <message>
        <location filename="../src/services/window-manager/x11/x11-window-manager.cpp" line="+424"/>
        <source>Desktop %1</source>
        <translation>데스크톱 %1</translation>
    </message>
</context>
<context>
    <name>XdpFileChooser</name>
    <message>
        <location filename="../src/services/file-chooser/xdp-file-chooser/xdp-file-chooser.cpp" line="+48"/>
        <source>Save</source>
        <translation>저장</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Save as</source>
        <translation>다른 이름으로 저장</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Select</source>
        <translation>선택</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Select a directory</source>
        <translation>디렉터리 선택</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Select files</source>
        <translation>파일 선택</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Select a file</source>
        <translation>파일 선택</translation>
    </message>
</context>
<context>
    <name>XxHotkeyGlobalShortcutBackend</name>
    <message>
        <location filename="../src/services/global-shortcuts/xx-hotkey-global-shortcut-backend.cpp" line="+108"/>
        <source>Unsupported trigger key</source>
        <translation>지원하지 않는 트리거 키</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Compositor does not support global hotkeys</source>
        <translation>컴포지터에서 전역 단축키를 지원하지 않습니다</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Hotkey binding was lost</source>
        <translation>단축키 연결이 해제되었습니다</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Compositor denied the bind. Try another key combination.</source>
        <translation>컴포지터에서 키 바인딩을 거부했습니다. 다른 키 조합을 시도하세요.</translation>
    </message>
</context>
<context>
    <name>browser-extension</name>
    <message>
        <location filename="../src/builtins/browser/browser-extension.cpp" line="-58"/>
        <source>No browser connected</source>
        <translation>연결된 브라우저 없음</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>You need to connect at least one browser to vicinae using the browser extension in order to use this command.</source>
        <translation>이 명령을 사용하려면 브라우저 확장 프로그램을 통해 하나 이상의 브라우저를 Vicinae에 연결해야 합니다.</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>Open documentation</source>
        <translation>문서 열기</translation>
    </message>
</context>
<context>
    <name>clipboard-history-view-host</name>
    <message>
        <location filename="../src/builtins/clipboard/history/clipboard-history-view-host.cpp" line="-232"/>
        <source>Text</source>
        <translation>텍스트</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Link</source>
        <translation>링크</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Image</source>
        <translation>이미지</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>File</source>
        <translation>파일</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Unknown</source>
        <translation>알 수 없음</translation>
    </message>
</context>
<context>
    <name>emoji-categories</name>
    <message>
        <location filename="../src/builtins/vicinae/emoji-grid-model.cpp" line="-52"/>
        <source>Smileys &amp; Emotion</source>
        <translation>웃는 얼굴 및 감정</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>People &amp; Body</source>
        <translation>사람 및 신체</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Animals &amp; Nature</source>
        <translation>동물 및 자연</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Food &amp; Drink</source>
        <translation>음식 및 음료</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Travel &amp; Places</source>
        <translation>여행 및 장소</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Activities</source>
        <translation>활동</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Objects</source>
        <translation>사물</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Symbols</source>
        <translation>기호</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Flags</source>
        <translation>깃발</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Math</source>
        <translation>수학</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Arrows</source>
        <translation>화살표</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Currency</source>
        <translation>통화</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Punctuation</source>
        <translation>문장 부호</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Shapes</source>
        <translation>도형</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Misc Symbols</source>
        <translation>기타 기호</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Greek</source>
        <translation>그리스 문자</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Number Forms</source>
        <translation>숫자 형태</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Fancy Letters</source>
        <translation>장식 문자</translation>
    </message>
</context>
<context>
    <name>emoji-grid-model</name>
    <message>
        <location line="+131"/>
        <source>Copy</source>
        <translation>복사</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Copy name</source>
        <translation>이름 복사</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Copy unicode codepoint</source>
        <translation>유니코드 코드 포인트 복사</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Copy category</source>
        <translation>카테고리 복사</translation>
    </message>
    <message>
        <location line="+40"/>
        <source>Skin tones</source>
        <translation>피부색</translation>
    </message>
</context>
<context>
    <name>file-list-item</name>
    <message>
        <location filename="../src/utils/file-list-item.hpp" line="+100"/>
        <source>Copy file</source>
        <translation>파일 복사</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Copy file path</source>
        <translation>파일 경로 복사</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Copy file name</source>
        <translation>파일 이름 복사</translation>
    </message>
    <message>
        <location line="+20"/>
        <source>Copy mime type</source>
        <translation>MIME 유형 복사</translation>
    </message>
</context>
<context>
    <name>font-categories</name>
    <message>
        <location filename="../src/services/font-service/font-service.cpp" line="+142"/>
        <source>Latin</source>
        <translation>라틴 문자</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Cyrillic</source>
        <translation>키릴 문자</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Greek</source>
        <translation>그리스 문자</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Monospace</source>
        <translation>고정폭</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Emoji</source>
        <translation>이모지</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Japanese</source>
        <translation>일본어</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Korean</source>
        <translation>한국어</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Simplified Chinese</source>
        <translation>중국어 간체</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Traditional Chinese</source>
        <translation>중국어 번체</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Arabic</source>
        <translation>아랍 문자</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Hebrew</source>
        <translation>히브리 문자</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Thai</source>
        <translation>태국 문자</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Lao</source>
        <translation>라오 문자</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Devanagari</source>
        <translation>데바나가리 문자</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Bengali</source>
        <translation>벵골 문자</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Gurmukhi</source>
        <translation>구르무키 문자</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Gujarati</source>
        <translation>구자라트 문자</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Tamil</source>
        <translation>타밀 문자</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Telugu</source>
        <translation>텔루구 문자</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Kannada</source>
        <translation>칸나다 문자</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Malayalam</source>
        <translation>말라얄람 문자</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Sinhala</source>
        <translation>싱할라 문자</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Armenian</source>
        <translation>아르메니아 문자</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Georgian</source>
        <translation>조지아 문자</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Thaana</source>
        <translation>타나 문자</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Tibetan</source>
        <translation>티베트 문자</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Myanmar</source>
        <translation>미얀마 문자</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Khmer</source>
        <translation>크메르 문자</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Syriac</source>
        <translation>시리아 문자</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Ogham</source>
        <translation>오검 문자</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Runic</source>
        <translation>룬 문자</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>N&apos;Ko</source>
        <translation>응코 문자</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Symbols</source>
        <translation>기호</translation>
    </message>
</context>
<context>
    <name>font-grid-model</name>
    <message>
        <location filename="../src/builtins/font/font-grid-model.cpp" line="+26"/>
        <source>Copy font family</source>
        <translation>글꼴 모음 복사</translation>
    </message>
</context>
<context>
    <name>keybind-manager</name>
    <message>
        <location filename="../src/internal/keyboard/keybind-manager.cpp" line="+9"/>
        <source>Toggle action panel</source>
        <translation>동작 패널 표시 전환</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Toggle the action panel to access and filter through the list of available actions for the currently selected item</source>
        <translation>동작 패널을 열거나 닫아 현재 선택한 항목에서 사용할 수 있는 동작 목록을 확인하고 필터링합니다</translation>
    </message>
    <message>
        <location line="+10"/>
        <source>Open Search Filter</source>
        <translation>검색 필터 열기</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Open the search filter selector if present</source>
        <translation>검색 필터 선택기가 있으면 엽니다</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Open settings window</source>
        <translation>설정 창 열기</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Open this settings window from the launcher window</source>
        <translation>런처 창에서 이 설정 창을 엽니다</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Generic Open Action</source>
        <translation>일반 열기 동작</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Can be used by actions that can open the selected item</source>
        <translation>선택한 항목을 여는 동작에 사용할 수 있습니다</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Generic Copy Action</source>
        <translation>일반 복사 동작</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Can be used by actions that can copy the selected item</source>
        <translation>선택한 항목을 복사하는 동작에 사용할 수 있습니다</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Copy Name Action</source>
        <translation>이름 복사 동작</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Can be used by actions that can copy the name of the selected item</source>
        <translation>선택한 항목의 이름을 복사하는 동작에 사용할 수 있습니다</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Copy Path Action</source>
        <translation>경로 복사 동작</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Can be used by actions that can copy the path of the selected item</source>
        <translation>선택한 항목의 경로를 복사하는 동작에 사용할 수 있습니다</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Save Action</source>
        <translation>저장 동작</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Can be used by actions that can save the selected item</source>
        <translation>선택한 항목을 저장하는 동작에 사용할 수 있습니다</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Duplicate Action</source>
        <translation>복제 동작</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Can be used by actions that can duplicate the selected item</source>
        <translation>선택한 항목을 복제하는 동작에 사용할 수 있습니다</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Generic New Action</source>
        <translation>일반 새로 만들기 동작</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Can be used by actions that create something</source>
        <translation>무언가를 새로 만드는 동작에 사용할 수 있습니다</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Generic Move Up Action</source>
        <translation>일반 위로 이동 동작</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Can be used by actions that can move up the selected item. This does not affect list navigation controls.</source>
        <translation>선택한 항목을 위로 옮기는 동작에 사용할 수 있습니다. 목록 탐색에는 영향을 주지 않습니다.</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Generic Move Down Action</source>
        <translation>일반 아래로 이동 동작</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Can be used by actions that can move down the selected item. This does not affect list navigation controls.</source>
        <translation>선택한 항목을 아래로 옮기는 동작에 사용할 수 있습니다. 목록 탐색에는 영향을 주지 않습니다.</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Generic Refresh Action</source>
        <translation>일반 새로고침 동작</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Can be used by actions that can refresh the selected item</source>
        <translation>선택한 항목을 새로고침하는 동작에 사용할 수 있습니다</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Generic Pin Action</source>
        <translation>일반 고정 동작</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Can be used by actions that can pin the selected item</source>
        <translation>선택한 항목을 고정하는 동작에 사용할 수 있습니다</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Remove Action</source>
        <translation>삭제 동작</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Can be used by actions that can remove the selected item. This is normally used for small, not too impactful removals.</source>
        <translation>선택한 항목을 삭제하는 동작에 사용할 수 있습니다. 일반적으로 영향이 크지 않은 간단한 삭제에 사용합니다.</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Dangerous Remove Action</source>
        <translation>주의가 필요한 삭제 동작</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Can be used by actions that perform an impactful removal, generally accompanied by a confirmation dialog.</source>
        <translation>영향이 큰 삭제를 수행하는 동작에 사용할 수 있으며 일반적으로 확인 대화 상자가 표시됩니다.</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Edit Action</source>
        <translation>편집 동작</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Can be used by actions that can edit the currently selected item</source>
        <translation>현재 선택한 항목을 편집하는 동작에 사용할 수 있습니다</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Edit Secondary Action</source>
        <translation>보조 편집 동작</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Can be used by actions that can edit a secondary characteristic of the currently selected item</source>
        <translation>현재 선택한 항목의 부가 속성을 편집하는 동작에 사용할 수 있습니다</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>Run as Administrator</source>
        <translation>관리자 권한으로 실행</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Runs the application with elevated privileges</source>
        <translation>앱을 관리자 권한으로 실행합니다</translation>
    </message>
</context>
<context>
    <name>media-extension</name>
    <message>
        <location filename="../src/builtins/media/media-extension.hpp" line="-198"/>
        <source>%1 — %2</source>
        <translation>%1 — %2</translation>
    </message>
    <message>
        <location line="+35"/>
        <source>No media player is running</source>
        <translation>실행 중인 미디어 플레이어가 없습니다</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>No media player matches &quot;%1&quot;</source>
        <translation>&quot;%1&quot;과 일치하는 미디어 플레이어가 없습니다</translation>
    </message>
    <message>
        <location line="+129"/>
        <source>Volume %1%</source>
        <translation>음량 %1%</translation>
    </message>
</context>
<context>
    <name>shortcut-conflict</name>
    <message>
        <location filename="../src/ui/settings/shortcut-conflict.cpp" line="+10"/>
        <source>Modifier required</source>
        <translation>보조 키 필요</translation>
    </message>
    <message>
        <location line="+4"/>
        <location line="+5"/>
        <source>Already bound to &quot;%1&quot;</source>
        <translation>이미 &quot;%1&quot;에 연결되어 있습니다</translation>
    </message>
</context>
<context>
    <name>utils</name>
    <message>
        <location filename="../src/utils/utils.cpp" line="+88"/>
        <source>0 bytes</source>
        <translation>0바이트</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>bytes</source>
        <translation>바이트</translation>
    </message>
</context>
<context>
    <name>virtual-desktops</name>
    <message>
        <location filename="../src/services/window-manager/windows/virtual-desktops.cpp" line="+67"/>
        <source>Desktop %1</source>
        <translation>데스크톱 %1</translation>
    </message>
</context>
</TS>
