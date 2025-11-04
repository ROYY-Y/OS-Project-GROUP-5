FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

# ติดตั้ง g++ และ build-essential
RUN apt-get update && \
    apt-get install -y g++ build-essential && \
    rm -rf /var/lib/apt/lists/*

WORKDIR /usr/src/app

# Copy โค้ด Server และ Client
COPY . .

# เปิด port สำหรับ server เผื่อ
EXPOSE 3000

# ไม่ต้องรัน CMD ใด ๆ ให้ container แค่พร้อมเท่านั้น
CMD ["bash"]
