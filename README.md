# IOCP Chatting System

Windows **IOCP 기반 비동기 네트워크 서버**와 **MFC 클라이언트**를 활용하여 구현한 채팅 시스템입니다.

이 프로젝트는 단순한 채팅 기능 구현을 넘어 다음과 같은 목표로 개발되었습니다.

- IOCP 기반 **비동기 네트워크 서버 구조 구현**
- 다수의 클라이언트를 동시에 처리하는 **세션 관리 시스템**
- **UI 이벤트 / 서비스 로직 / 네트워크 처리 구조 분리**
- 실제 서비스 구조에 가까운 **DB 연동 채팅 시스템**

---

# Development Status

| Module | Status |
|------|------|
| Server Core | Completed |
| Server Logic | Completed |
| MFC Client | In Progress |

---

# Tech Stack

## Language
- C++

## Network
- Winsock2
- IOCP (I/O Completion Port)

## Client
- MFC

## Database
- MySQL
- ODBC

## Architecture
- Session 기반 연결 관리
- Packet 기반 통신
- Handler / Service 구조

---

