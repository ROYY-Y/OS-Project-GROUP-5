README - NoLock
ระบบ Chatroom Server (NoLock Version) เป็นเวอร์ชันพื้นฐานของระบบแชตหลายห้อง ที่ตัดกลไก lock และการป้องกัน thread ออก เพื่อเเสดงให้เห็นถึงหลักการทำงานที่threadหลายตัว เข้าถึงข้อมูลเดียวกันพร้อมกัน
และเกิดปัญหา Race Condition
(No_lock_Server.cpp)
ส่วนประกอบหลัก
1.Data Structures
-Client
เก็บข้อมูลของผู้ใช้แต่ละคน เช่น
sockfd : Socket descriptor สำหรับเชื่อมต่อ
mail : คิวเก็บข้อความที่ต้องส่งออก (queue แบบไม่ lock)
id : หมายเลขประจำตัว client
currRoom : ห้องที่อยู่ปัจจุบัน

-Room
เก็บสมาชิกภายในห้อง (unordered_map<int, Client>)
ไม่มีการป้องกันการเข้าถึงพร้อมกัน (no mutex)

-Rooms (global)
เก็บห้องทั้งหมดในระบบ
ไม่มีการใช้ lock ป้องกัน (อาจเกิด race condition ได้หากหลาย thread เข้าถึงพร้อมกัน)
ห้องเริ่มต้นของระบบคือ "Lobby"

2.Threads ที่ทำงานในระบบ
-client_Thread
รับคำสั่งจาก Client (เช่น JOIN, SAY, DM, WHO, LEAVE) แล้วประมวลผลตาม logic

-send_Thread
ส่งข้อความจาก queue (mail) ไปยัง Client
ไม่มี condition_variable หรือ mutex — ใช้ sleep_for() หน่วงเวลาเล็กน้อยแทน

-server_Check (Admin Thread)
รับคำสั่งจากฝั่ง server console เช่น
trm : แสดงจำนวนห้องทั้งหมด
lsrm : แสดงรายชื่อห้องและจำนวนสมาชิกในแต่ละห้อง
----------------------------------------------------------------------------------------------------------------------------------------
Feature
1.No Lock (Unprotected Access)
-ไม่มี mutex / condition_variable ป้องกัน
-ลดการรอคิวของ thread อื่น เพิ่มประสิทธิภาพในระบบที่มีการส่งข้อความจำนวนมากพร้อมกันเเต่มีความเสี่ยง race condition ถ้ามีหลาย thread เข้าถึงข้อมูลพร้อมกัน

2.Automatic Room Management
-สร้างห้องอัตโนมัติเมื่อมี JOIN ห้องใหม่
-ลบห้องอัตโนมัติเมื่อไม่มีสมาชิก (ยกเว้น Lobby)

3.System Notifications
-แจ้งเมื่อมีสมาชิกเข้า/ออกห้อง
-Broadcast ข้อความแบบอัตโนมัติให้สมาชิกในห้อง
----------------------------------------------------------------------------------------------------------------------------------------
Requirement
C++17 หรือใหม่กว่า
POSIX-compliant OS (Linux/Unix/macOS)
pthread library


