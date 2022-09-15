# MyWebRequest

https://user-images.githubusercontent.com/90883558/136732286-9b16673f-cfd6-4a36-b2b7-76a85f39cc18.mp4

■ curl 을 사용해 GET 과 POST 하기

- Mac OS, Linux 에는 curl 이 기본 탑재되어있다	-->curl 로 http request 가능

- Windows10 이후부터 curl 이 탑재되어 있다 --> Invoke-WebRequest 커맨드, curl 로 http request가능

- 만약 curl 패키지를 사용할수 없다면 curl을 새로 설치하도록 하자
--------------------------------------------------------------------------------------------
☆설치방법 1
- https://chocolatey.org/install 에서 초코 패키지 설치
	-> 파워쉘 관리자 모드로 실행 후 커맨드 따라 친다
커맨드 :
Set-ExecutionPolicy Bypass -Scope Process -Force; [System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072; iex ((New-Object System.Net.WebClient).DownloadString('https://community.chocolatey.org/install.ps1'))

- 설치 후 커맨드 입력
choco install curl

-설치완료
------------------------------------------------------------------------------------
☆설치방법 2
-https://curl.haxx.se/download.html
위에서 Windows 64bit --- binary the curl project 다운

- C\\ 압축해제

- curl.exe 위치를 환경변수에 추가

- 어디에서나 curl을 사용할수 있게 되었다
 
- 설치완료
--------------------------------------------------------------------------------------------
■ GET 오류

Invoke-WebRequest : 매개 변수 이름 'X'과(와) 일치하는 매개 변수를 찾을 수 없습니다.
위치 줄:1 문자:6
+ curl -X GET http://127.0.0.1:8000
+      ~~
    + CategoryInfo          : InvalidArgument: (:) [Invoke-WebRequest], ParameterBindingException
    + FullyQualifiedErrorId : NamedParameterNotFound,Microsoft.PowerShell.Commands.InvokeWebRequestCommand

-- 위와 같은 오류 발생시

powershell --> remove-item alias:\curl


---------------------------------------------------------------------------------------------------
■ POST 오류
위치 줄:1 문자:47
+ curl -X POST http://127.0.0.1:8000/abc.txt -d @test.txt
+                                               ~~~~~
식에서 변수를 참조하는 데 스플랫(splat) 연산자 '@'를 사용할 수 없습니다. 
'@test'은(는) 명령 인수로만 사용할 수 있습니다. 식에서 변수를 참조하려면 '$test'을(를) 사용하십시오.
    + CategoryInfo          : ParserError: (:) [], ParentContainsErrorRecordException
    + FullyQualifiedErrorId : SplattingNotPermitted

POST 실행시 위와 같은 오류 발생시

커맨드 : curl -X POST http://127.0.0.1:8000/abc.txt -d @test.txt

--> Windows Powershell 에서 @ 문자가 다른 기능으로 인식될수 있다.

--> @test.txt 부분을 "@test.txt" 로 감싸면 해결된다.

--> curl -X POST http://127.0.0.1:8000/abc.txt -d "@test.txt"

--> "test.txt"의 내용을 복사한 "abc.txt."파일 생성
------------------------------------------------------------------------------------------------
