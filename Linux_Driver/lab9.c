#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // 用於 usleep()

#define GPIO_EXPORT_PATH "/sys/class/gpio/export"
#define GPIO_DIRECTION_PATH "/sys/class/gpio/gpio4/direction"
#define GPIO_VALUE_PATH "/sys/class/gpio/gpio4/value"
#define GPIO_UNEXPORT_PATH "/sys/class/gpio/unexport"

// 開啟 GPIO
void export_gpio() {
    FILE *p = fopen(GPIO_EXPORT_PATH, "w");
    if (p == NULL) {
        perror("Failed to open GPIO export");
        exit(EXIT_FAILURE);
    }
    fprintf(p, "%d", 4);
    fclose(p);
}

// 設置 GPIO 方向為輸出
void set_gpio_direction() {
    FILE *p = fopen(GPIO_DIRECTION_PATH, "w");
    if (p == NULL) {
        perror("Failed to open GPIO direction");
        exit(EXIT_FAILURE);
    }
    fprintf(p, "out");
    fclose(p);
}

// 設置 GPIO 值
void set_gpio_value(int value) {
    FILE *p = fopen(GPIO_VALUE_PATH, "w");
    if (p == NULL) {
        perror("Failed to open GPIO value");
        exit(EXIT_FAILURE);
    }
    fprintf(p, "%d", value);
    fclose(p);
}

// 關閉 GPIO
void unexport_gpio() {
    FILE *p = fopen(GPIO_UNEXPORT_PATH, "w");
    if (p == NULL) {
        perror("Failed to open GPIO unexport");
        exit(EXIT_FAILURE);
    }
    fprintf(p, "%d", 4);
    fclose(p);
}

int main() {
    // 持續閃爍 LED
    while (1) {
        // 開啟 GPIO
        export_gpio();
        // 設置 GPIO 方向為輸出
        set_gpio_direction();
        // 設置 GPIO 值為高電平，點亮 LED
        set_gpio_value(1);
        // 休眠 1 秒
        usleep(1000000);
        // 設置 GPIO 值為低電平，熄滅 LED
        set_gpio_value(0);
        // 關閉 GPIO
        unexport_gpio();
        // 休眠 1 秒
        usleep(1000000);
    }
    return 0;
}
