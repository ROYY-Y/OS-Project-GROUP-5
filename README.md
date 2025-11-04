README - Chatroom Server Project
ระบบ Chatroom แบบ Multi-threaded ที่ใช้ TCP Socket รองรับหลาย Client พร้อมกัน

ส่วนประกอบหลัก มี 2 ส่วน คือ
1. Server.cpp
	Data Structures:
		Client -> เก็บ socket, message queue, mutex, condition variable, ID, ห้องปัจจุบัน
		Room -> เก็บรายชื่อสมาชิก + room mutex
		Rooms -> เก็บห้องทั้งหมด (Global map)
	Threads: (จะมี3 threads ต่อกับ Client)
		client_Thread -> รับและประมวลผลคำสั่งจาก Client (Router)
		send_Thread -> ส่งข้อความจาก queue ไปยัง Client (Broadcaster)
		server_Check -> รับคำสั่ง admin จาก server console
2. Client.cpp
	Threads: (จะมี2 threads)
		send_Thread -> รับ input จากผู้ใช้ ส่งไป Server
		recive_Thread -> รับข้อความจาก Server เพื่อไปแสดงผล
----------------------------------------------------------------------------------------------------------------------------------------
คำสั่งที่รองรับ มี 2 ฝั่ง(Client และ Server) คือ
1. Client Commands
	JOIN <room_name> เข้าห้อง (สร้างใหม่ถ้ายังไม่มี)
	SAY <message> ส่งข้อความในห้อง (broadcast)
	DM <client_id> <msg> ส่งข้อความส่วนตัว (direct message)
	WHO ดูสมาชิกในห้อง (แสดงรายชื่อสมาชิก)
	INFO ดู Commands ทั้งหมดที่ Client สามารถใช้ได้
	LEAVE กลับไป Lobby
	QUIT ออกจากระบบ
2. Server Admin Commands
	trm ดูจำนวนห้องทั้งหมด
	lsrm ดูรายชื่อห้อง + จำนวนคนในแต่ละห้อง
----------------------------------------------------------------------------------------------------------------------------------------
วิธีการใช้งาน
Compile:
g++ -std=c++17 -pthread Server.cpp -o server
g++ -std=c++17 -pthread Client.cpp -o client

Run:
Terminal 1 (ของ Server):
./server

Terminal 2 ขึ้นไป (ของ Clients): Client จะเชื่อมต่อ localhost:3000 และเริ่มที่ห้อง Lobby
./client
----------------------------------------------------------------------------------------------------------------------------------------
ตัวอย่างการใช้งาน
Client 1:
	JOIN RoomABC
	SAY Hello!
	WHO

Client 2:
	JOIN RoomABC
	SAY Hi!
	DM 1 Private message to you

Server Console:
	lsrm
	trm
----------------------------------------------------------------------------------------------------------------------------------------
Feature ที่สำคัญนอกจากคำสั่งหลัก
Automatic Room Management:
	- สร้างห้องอัตโนมัติเมื่อ JOIN ห้องใหม่
	- ลบห้องว่างอัตโนมัติ (ยกเว้น Lobby)
System Notifications:
	- แจ้งเตือนเมื่อมีคนเข้า/ออกห้อง
	- แจ้ง error เมื่อใช้คำสั่งผิด
Thread-Safe:
	- ใช้ mutex ป้องกัน race condition
	- ใช้ condition variable ลด CPU usage
----------------------------------------------------------------------------------------------------------------------------------------
กลไกการทำ Synchronization
Mutex Locks:
	Client::mx			ป้องกัน mail queue
	Room::room_mtx		ป้องกัน member list
	Rooms_mtx			ป้องกัน global rooms map
Condition Variables:
	- ใช้ใน send_Thread เพื่อรอข้อความใหม่
	- หลีกเลี่ยง busy-waiting
----------------------------------------------------------------------------------------------------------------------------------------
Requirements
	- C++17 or higher
	- POSIX-compliant OS (Linux/Unix/macOS)
	- pthread library
