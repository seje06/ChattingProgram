# IOCP Chatting System

Windows IOCP 기반 비동기 네트워크 서버와 MFC 클라이언트를 활용하여 구현한 채팅 시스템입니다.

회원가입, 로그인, 채팅방 생성/입장, 실시간 채팅 기능을 포함하며  
다수의 클라이언트를 동시에 처리할 수 있는 **비동기 서버 구조**를 직접 구현했습니다.

---

# Development Status

### Server Core : Completed
### MFC Client : In Progress

---

# Development Environment

### Language
- C++

### Network
- Winsock2
- IOCP (I/O Completion Port)

### Client
- MFC

### Database
- MySQL
- ODBC

### Other
- Multithreading
- Session Management
- Packet-based Communication

---

# System Architecture

클라이언트 UI 이벤트는 Handler를 통해 서비스 계층으로 전달되며  
서비스 계층은 서버와 통신하고 DB와 연동하여 결과를 반환합니다.

[MFC Client]

      │
      ▼

[Handler Layer]

      │
      ▼

[Contents Service]

      │
      ▼

[IOCP Server]

      │
      ▼

[MySQL Database]













