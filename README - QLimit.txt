README - QLimit
ระบบ Chatroom Server แบบที่เพิ่มกลไก “Queue Limit (QLimit)” เพื่อจำกัดจำนวนข้อความในคิวของแต่ละ Client ป้องกันการ overload และเพิ่มความเสถียรของระบบ
(QLimit_Server.cpp)
ส่วนประกอบหลัก
1.Data Structures
- Client
เก็บข้อมูลของผู้ใช้แต่ละคน เช่น
sockfd: Socket descriptor
mail: คิวข้อความ (queue) สำหรับส่งออก
mx + cv: ใช้ควบคุมการเข้าถึงคิวอย่างปลอดภัย (thread-safe)
id: หมายเลขประจำตัว client
currRoom: ห้องที่อยู่ปัจจุบัน

-Room
เก็บสมาชิกภายในห้อง (unordered_map<int, Client>)
พร้อม mutex (room_mtx) สำหรับป้องกัน race condition

-Rooms (global)
เก็บห้องทั้งหมดในระบบ
ป้องกันด้วย mutex Rooms_mtx
ห้องเริ่มต้นของระบบคือ "Lobby"

2.Threads ที่ทำงานในระบบ
-client_Thread
รับคำสั่งจาก Client (เช่น JOIN, SAY, DM, WHO, LEAVE) แล้วประมวลผลตาม logic

-send_Thread
ทำหน้าที่ส่งข้อความจาก mail queue ไปยัง Client
ใช้ condition variable เพื่อรอข้อความใหม่อย่างมีประสิทธิภาพ (ไม่ busy-wait)

-server_Check (Admin Thread)
รับคำสั่งจากฝั่ง server console เช่น
trm : แสดงจำนวนห้องทั้งหมด
lsrm : แสดงรายชื่อห้องและจำนวนสมาชิกในแต่ละห้อง
----------------------------------------------------------------------------------------------------------------------------------------
Feature
1.Queue Limit (QLimit)
-จำกัดจำนวนข้อความที่ Client แต่ละคนจะเก็บได้ สามารถระบุจำนวน max เท่าไหร่ก็ได้ โดยในตัวอย่างจะระบุเท่ากับ5(MAX_MAIL = 5)
-หากเกินกว่าที่กำหนด จะเตือนผู้ส่งด้วยข้อความ 
[Server Warning] : You send too many message! Please wait for messages to be delivered.
หมายความว่า คิวของผู้รับเต็มอยู่ ตอนนี้ระบบจะยังไม่ส่งข้อความต่อ จนกว่าคิวของClientปลายทางจะว่าง จึงจะสามารถส่งข้อความใหม่ได้อีกครั้ง

2.Automatic Room Management
-สร้างห้องอัตโนมัติเมื่อมี JOIN ห้องใหม่
-ลบห้องอัตโนมัติเมื่อไม่มีสมาชิก (ยกเว้น Lobby)

3.System Notifications
-แจ้งเมื่อมีสมาชิกเข้า/ออกห้อง
-Broadcast ข้อความแบบอัตโนมัติให้สมาชิกในห้อง

4.Thread-Safe Synchronization
-ใช้ std::mutex ป้องกัน race condition
-ใช้ std::condition_variable ลด CPU usage (แทน busy waiting)
----------------------------------------------------------------------------------------------------------------------------------------
Requirement
C++17 หรือใหม่กว่า
POSIX-compliant OS (Linux/Unix/macOS)
pthread library



