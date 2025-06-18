#include <stdio.h>      // Thư viện nhập xuất chuẩn
#include <stdlib.h>     // Thư viện cấp phát bộ nhớ, malloc, free
#include <string.h>     // Thư viện xử lý chuỗi
#include <regex.h>      // Thư viện biểu thức chính quy (POSIX)

// ----- Contains ----- //
#define MAX_LINE_LENGTH 256   // Độ dài tối đa cho 1 dòng
#define MAX_LINES 1000        // Số dòng tối đa có thể đọc

// Hàm đọc file và lưu từng dòng vào mảng line[]
// Trả về số dòng đã đọc thành công hoặc -1 nếu lỗi
int readFileTxt(const char* filename, char* line[]) {
    int lineCount = 0;  // Biến đếm số dòng đã đọc
    FILE* fp = fopen(filename, "r"); // Mở file để đọc

    if (fp == NULL) {
        printf("Could not open file '%s'\n", filename);
        return -1;  // Lỗi: không mở được file
    }

    char buffer[MAX_LINE_LENGTH]; // Bộ đệm để đọc từng dòng

    // Đọc từng dòng từ file
    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        size_t len = strlen(buffer);  // Lấy độ dài dòng vừa đọc

        // Nếu dòng kết thúc bằng '\n', thay bằng '\0'
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
            len--; // giảm độ dài thật sự
        }

        // Cấp phát bộ nhớ cho dòng này
        line[lineCount] = new char[len + 1]; // +1 cho '\0'

        if (line[lineCount] == NULL) {
            printf("Could not allocate memory at line %d\n", lineCount);
            fclose(fp);
            return -1;
        }

        // Sao chép nội dung dòng vào vùng nhớ đã cấp phát
        strcpy(line[lineCount], buffer);

        printf("Line %d: %s\n", lineCount + 1, line[lineCount]);

        lineCount++; // Tăng số dòng đếm được
    }

    fclose(fp); // Đóng file lại
    return lineCount; // Trả về số dòng đọc thành công
}

// Hàm kiểm tra mỗi dòng có bắt đầu bằng từ "Requirements" không
int checkRegexMatch(char* line[], char* matchArray[], int lineCount) {
    regex_t regex; // Cấu trúc regex
    int reti;
    int matchCount = 0;

    // Tạo biểu thức chính quy: dòng bắt đầu bằng "Requirements"
    reti = regcomp(&regex, "(Requirements|requirement)", REG_EXTENDED | REG_NOSUB | REG_ICASE);
    if (reti) {
        printf("Could not compile regex\n");
        return -1;
    }

    printf("Checking line with regex...\n");

    for (int i = 0; i < lineCount; i++) {
        // So khớp dòng hiện tại với biểu thức chính quy
        // regexec sẽ trả về 0 nếu dòng phù hợp với regex
        // Nếu không phù hợp, nó sẽ trả về một giá trị khác (khác 0)
        // Trong trường hợp này, chúng ta không cần thông tin chi tiết về vị trí khớp
        // nên truyền NULL cho tham số thứ 3 và 4
        // Tham số thứ 5 là 0 để không sử dụng các tùy chọn bổ sung
        printf("Checking line %d: %s\n", i + 1, line[i]);
        reti = regexec(&regex, line[i], 0, NULL, 0);

        if (reti == 0) {
            printf("=> Line %d MATCHES: %s\n", i + 1, line[i]);
            matchArray[matchCount] = new char[strlen(line[i]) + 1]; // Cấp phát bộ nhớ cho dòng phù hợp
            if (matchArray[matchCount] == NULL) {
                printf("Could not allocate memory at line %d\n", i + 1);
                regfree(&regex); // Giải phóng regex trước khi thoát
                return -1;
            }
            strcpy(matchArray[matchCount], line[i]); // Sao chép dòng phù hợp vào mảng matchArray
            printf("Line %d saved to matchArray: %s\n", i + 1, matchArray[matchCount]);
            matchCount++;
        } else {
            printf("Line %d DOES NOT MATCH: %s\n", i + 1, line[i]);
        }
    }

    // Giải phóng bộ nhớ regex
    regfree(&regex);

    return matchCount; // Trả về số dòng phù hợp
}

void createFileTxt(const char* filename, char* matchArray[], int macthCount) {
    FILE* fp = fopen(filename, "w");
    if (fp == NULL) {
        printf("Could not open file %s\n", filename);
        return;
    }

    for (int i = 0; i < macthCount; i++)
    {
        fprintf(fp, "%s\n", matchArray[i]); // Ghi từng dòng vào file
    }
    
    fclose(fp);
    // In ra thông báo đã tạo file thành công
    if (macthCount == 0) {
        printf("No lines matched the regex. File '%s' is empty.\n", filename);
    } else if (macthCount == 1)
    {
        printf("Created file '%s' with %d line.\n", filename, macthCount);
    } else {
        printf("Created file '%s' with %d lines.\n", filename, macthCount);
    }
}
// Hàm giải phóng vùng nhớ cấp phát cho từng dòng
void freeLines(char* line[], int count) {
    for (int i = 0; i < count; i++) {
        delete[] line[i];  // Giải phóng từng dòng
    }
}

// Hàm chính của chương trình
int main() {
    char* line[MAX_LINES]; // Mảng con trỏ chứa các dòng văn bản
    char* matchArray[MAX_LINES]; // Mảng chứa các dòng phù hợp với regex

    printf("Hello Duy!\n");

    // Gọi hàm đọc file
    int lineCount = readFileTxt("input.txt", line);
    if (lineCount < 0) {
        printf("Could not open file.\n");
        return 1;
    }

    printf("Total lines: %d\n", lineCount);

    // Gọi hàm kiểm tra biểu thức chính quy
    int matchCount = checkRegexMatch(line, matchArray, lineCount);
    printf("Total matched lines: %d\n", matchCount);

    // Tạo file mới với nội dung đã đọc
    createFileTxt("output.txt", matchArray, matchCount);

    // Giải phóng bộ nhớ
    freeLines(line, lineCount);

    printf("Program finished.\n");
    return 0;
}