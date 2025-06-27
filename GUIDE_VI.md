# Hệ Điều Hành Giải Phương Trình Bậc Hai - Hướng Dẫn Sinh Viên

## Dự án này là gì?

Đây là một **hệ điều hành đơn giản** có thể giải phương trình bậc hai! Nó giống như một máy tính, nhưng chạy trực tiếp trên máy tính mà không cần hệ điều hành khác (như Windows hay Linux) bên dưới.

## Tổng Quan - Điều gì xảy ra khi bạn khởi động HĐH này?

Hãy nghĩ về chuỗi các bước sau:
1. **Máy tính khởi động** → Boot loader chạy
2. **Boot loader** → Tải và khởi chạy kernel của chúng ta
3. **Kernel** → Thiết lập máy tính và bắt đầu chương trình giải phương trình
4. **Người dùng** → Nhập số và nhận kết quả!

## Cách Tổ Chức Code

### 📁 Cấu Trúc Thư Mục
```
wyoos-lab13-keyboard/
├── src/
│   ├── loader.s          # Boot loader (Assembly)
│   ├── kernel.cpp        # Code kernel chính
│   ├── gdt.cpp          # Thiết lập quản lý bộ nhớ
│   ├── interrupts.cpp   # Xử lý đầu vào bàn phím
│   ├── interrupts.s     # Xử lý ngắt bằng Assembly
│   ├── keyboard.cpp     # Xử lý những gì bạn gõ
│   └── port.cpp         # Giao tiếp với phần cứng
├── Makefile             # Hướng dẫn build
└── README.md           # Thông tin dự án
```

## Hành Trình: Từ Khởi Động Đến Giải Phương Trình

### 🚀 Bước 1: Quá Trình Khởi Động (`loader.s:15-25`)
**Chức năng:** Đây là đoạn code đầu tiên chạy khi bạn khởi động HĐH.

```assembly
loader:
    mov $kernel_stack, %esp    # Thiết lập bộ nhớ stack
    call callConstructors     # Khởi tạo các đối tượng C++
    call kernelMain          # Nhảy tới kernel chính
```

**Tại sao cần thiết:** Máy tính cần được hướng dẫn cách khởi động. Code này chuẩn bị máy tính và sau đó gọi hàm kernel chính.

### 🧠 Bước 2: Khởi Tạo Kernel (`kernel.cpp:94-107`)
**Chức năng:** Thiết lập các thành phần cơ bản của hệ điều hành.

```cpp
extern "C" void kernelMain(void* multibootStruct, uint32_t magicNumber)
{
    GlobalDescriptorTable gdt;           // Thiết lập quản lý bộ nhớ
    InterruptManager interrupts(0x20, &gdt);  // Thiết lập xử lý bàn phím
    
    printf("Initializing system...\n"); // Báo cho người dùng đang khởi động
    interrupts.Activate();              // Bật đầu vào bàn phím
    StartQuadraticSolver();             // Khởi chạy chương trình chính
    
    while(1);  // Tiếp tục chạy mãi mãi
}
```

**Tại sao cần thiết:** Trước khi có thể sử dụng bàn phím hoặc hiển thị văn bản, chúng ta cần báo cho máy tính cách quản lý bộ nhớ và xử lý đầu vào.

### 🎯 Bước 3: Quản Lý Bộ Nhớ (`gdt.cpp:3-13`)
**Chức năng:** Tạo "bản đồ" bộ nhớ máy tính để các chương trình biết nơi có thể lưu trữ dữ liệu.

```cpp
GlobalDescriptorTable::GlobalDescriptorTable()
    : nullSegmentSelector(0, 0, 0),
      codeSegmentSelector(0, 64*1024*1024, 0x9A),    // Code có thể đặt ở đây
      dataSegmentSelector(0, 64*1024*1024, 0x92)     // Data có thể đặt ở đây
{
    // Báo cho CPU về cách bố trí bộ nhớ
    asm volatile("lgdt (%0)": :"p" (((uint8_t *) i)+2));
}
```

**Tại sao cần thiết:** Không có điều này, máy tính sẽ không biết nơi đặt code và dữ liệu của chương trình một cách an toàn.

### ⌨️ Bước 4: Thiết Lập Bàn Phím (`interrupts.cpp:20-58`)
**Chức năng:** Thiết lập máy tính để "lắng nghe" các phím được nhấn.

**Luồng hoạt động:**
1. **Thiết lập bảng ngắt** - Tạo danh sách việc cần làm khi phím được nhấn
2. **Cấu hình phần cứng** - Báo cho bộ điều khiển bàn phím cách hoạt động
3. **Kích hoạt ngắt** - Bắt đầu lắng nghe các phím được nhấn

**Tại sao cần thiết:** Không có điều này, máy tính sẽ không biết khi bạn nhấn phím!

### 🎹 Bước 5: Xử Lý Phím Được Nhấn (`interrupts.cpp:97-123`)
**Chức năng:** Khi bạn nhấn phím, code này xác định phím nào đã được nhấn.

```cpp
void InterruptManager::HandleKeyboardInterrupt()
{
    Port8Bit dataport(0x60);        // Đọc từ phần cứng bàn phím
    uint8_t key = dataport.Read();   // Lấy mã phím
    
    // Chuyển đổi mã phím thành ký tự thực
    static char keyboardmap[128] = {
        0,  27, '1', '2', '3', '4', '5', '6', '7', '8',
        '9', '0', '-', '=', '\b', '\t',
        'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p',
        // ... nhiều phím khác
    };
    
    if(key < 128 && keyboardmap[key] != 0) {
        ProcessKeyboardInput(keyboardmap[key]);  // Gửi tới bộ giải phương trình
    }
}
```

**Tại sao cần thiết:** Bàn phím gửi số (mã phím), nhưng chúng ta cần chữ cái và số thực cho chương trình.

### 🔢 Bước 6: Xử Lý Đầu Vào (`keyboard.cpp:13-81`)
**Chức năng:** Nhận các phím bạn nhấn và tạo thành số cho bộ giải phương trình.

**Luồng hoạt động:**
1. **Thu thập ký tự** - Tạo thành số khi bạn gõ
2. **Xử lý Enter** - Khi nhấn Enter, chuyển đổi văn bản thành số
3. **Lưu hệ số** - Lưu giá trị a, b, c cho phương trình
4. **Xử lý backspace** - Cho phép sửa lỗi

```cpp
void ProcessKeyboardInput(char key)
{
    if(key == '\n' || key == '\r') {  // Nhấn Enter
        // Chuyển đổi văn bản thành số
        int32_t number = 0;
        for(int i = start; i < input_pos; i++) {
            number = number * 10 + (input_buffer[i] - '0');
        }
        
        // Lưu vào a, b, hoặc c tùy theo giai đoạn
        switch(coefficient_stage) {
            case 0: a = number; break;
            case 1: b = number; break; 
            case 2: c = number; SolveQuadraticEquation(); break;
        }
    }
}
```

### 🧮 Bước 7: Giải Phương Trình (`keyboard.cpp:83-176`)
**Chức năng:** Sử dụng công thức nghiệm để tìm nghiệm.

**Toán học:**
- Cho phương trình ax² + bx + c = 0
- Biệt thức Δ = b² - 4ac
- Nếu Δ > 0: Hai nghiệm phân biệt
- Nếu Δ = 0: Một nghiệm kép
- Nếu Δ < 0: Không có nghiệm thực

```cpp
void SolveQuadraticEquation()
{
    int32_t discriminant = b * b - 4 * a * c;
    
    if(discriminant > 0) {
        printf("Phương trình có hai nghiệm phân biệt\n");
        // Tính x1 và x2
    }
    else if(discriminant == 0) {
        printf("Phương trình có một nghiệm kép\n");
        // Tính nghiệm kép
    }
    else {
        printf("Phương trình không có nghiệm thực\n");
    }
}
```

### 📺 Bước 8: Hiển Thị Kết Quả (`kernel.cpp:5-48`)
**Chức năng:** Hiển thị văn bản trên màn hình bằng cách ghi trực tiếp vào bộ nhớ video.

```cpp
void printf(const char* str)
{
    static uint16_t* VideoMemory = (uint16_t*)0xb8000;  // Bộ nhớ màn hình
    
    for(int i = 0; str[i] != '\0'; ++i) {
        VideoMemory[80*y+x] = str[i];  // Đặt ký tự lên màn hình
        x++;  // Chuyển tới vị trí tiếp theo
    }
}
```

**Tại sao cần thiết:** Đây là cách chúng ta hiển thị kết quả cho người dùng!

## Khái Niệm Chính Cho Bài Thuyết Trình

### 1. **Lập Trình Bare Metal**
- Không có Windows/Linux bên dưới
- Chúng ta giao tiếp trực tiếp với phần cứng
- Kiểm soát nhiều hơn, nhưng cũng có trách nhiệm hơn

### 2. **Đầu Vào Dựa Trên Ngắt**
- Máy tính "ngắt" chương trình khi phím được nhấn
- Giống như nhận cuộc gọi điện thoại khi đang làm bài tập
- Chúng ta xử lý "cuộc gọi" (phím được nhấn) rồi tiếp tục

### 3. **Quản Lý Bộ Nhớ**
- Chúng ta báo cho máy tính biết nơi code và dữ liệu được lưu
- Giống như sắp xếp bàn học trước khi bắt đầu làm bài tập

### 4. **Máy Trạng Thái**
- Chương trình nhớ hệ số nào (a, b, hay c) đang được nhập
- Giống như làm theo công thức nấu ăn từng bước một

## Câu Hỏi Thường Gặp & Trả Lời

**H: Tại sao dùng ngôn ngữ assembly?**
Đ: Một số tác vụ (như khởi động) phải được thực hiện bằng assembly vì chúng quá cấp thấp đối với C++.

**H: Ngắt là gì?**
Đ: Nó giống như chuông cửa - khi được nhấn, nó dừng việc bạn đang làm để xử lý khách (đầu vào bàn phím).

**H: Tại sao không thể dùng printf thông thường của C++?**
Đ: Chúng ta không có hệ điều hành để cung cấp printf, vì vậy phải tự viết!

**H: Máy tính biết chạy code của chúng ta như thế nào?**
Đ: Bootloader tuân theo giao thức tiêu chuẩn (multiboot) cho biết kernel bắt đầu từ đâu.

## Quy Trình Build

1. **Biên dịch** các file C++ → File đối tượng (.o)
2. **Assembler** các file Assembly → File đối tượng (.o)
3. **Liên kết** tất cả đối tượng → File thực thi nhị phân
4. **Tạo ISO** → File ảnh đĩa có thể khởi động
5. **Khởi động** trong máy ảo

## Tóm Tắt

HĐH này minh họa các khái niệm cơ bản của hệ điều hành:
- **Quá trình khởi động** - Cách máy tính khởi động
- **Tương tác phần cứng** - Giao tiếp với bàn phím và màn hình
- **Quản lý bộ nhớ** - Tổ chức bộ nhớ máy tính
- **Xử lý ngắt** - Phản hồi với các sự kiện bên ngoài
- **Tương tác người dùng** - Nhận đầu vào và hiển thị kết quả

Code chạy theo luồng: khởi động → thiết lập kernel → xử lý ngắt → đầu vào người dùng → giải phương trình → hiển thị kết quả, tạo thành một hệ thống tương tác hoàn chỉnh!

## Gợi Ý Thuyết Trình

### 🎯 Cấu Trúc Bài Thuyết Trình Đề Xuất:

1. **Giới thiệu** (2-3 phút)
   - Giải thích dự án là gì
   - Tại sao tạo HĐH riêng thay vì dùng chương trình thông thường

2. **Kiến trúc tổng quan** (3-4 phút)
   - Sơ đồ các thành phần chính
   - Luồng dữ liệu từ khởi động đến hiển thị kết quả

3. **Các thành phần kỹ thuật** (5-6 phút)
   - Boot loader và kernel
   - Quản lý bộ nhớ (GDT)
   - Hệ thống ngắt và xử lý bàn phím
   - Thuật toán giải phương trình

4. **Demo** (2-3 phút)
   - Chạy thực tế trên máy ảo
   - Nhập phương trình và xem kết quả

5. **Kết luận** (1-2 phút)
   - Những gì học được
   - Ứng dụng thực tế của các khái niệm

### 💡 Mẹo Thuyết Trình:
- Sử dụng sơ đồ và hình ảnh để minh họa
- Chuẩn bị video demo phòng trường hợp lỗi kỹ thuật
- Nhấn mạnh những khó khăn đã vượt qua
- Giải thích tại sao chọn cách tiếp cận này