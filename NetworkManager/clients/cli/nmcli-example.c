#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "libnmcli.h"

int main(int argc, char **argv)
{
    int result;

    printf("nmcli Library Example\n");
    printf("Version: %s\n\n", nmcli_get_version());

    printf("Library ready to use\n");

    /* Example 1: Simple command execution */
    /**/
    printf("\n=== Example 1: Simple command ===\n");
    char *args1[] = {"nmcli", "device", "status"};
    result = nmcli_execute(3, args1);
    printf("Result: %d\n", result);

    /* Example 2: Command with output capture */
    printf("\n=== Example 2: Command with output capture ===\n");
    char *args2[] = {"nmcli", "device", "status"};
    char output[4096];
    size_t size = sizeof(output);
    char error[256];
    size_t error_size = sizeof(error);
    int ret = nmcli_execute_with_output(3, args2, output, &size, error, &error_size);
    printf("ret: %d\n", ret);
    if (ret == NMC_RESULT_ERROR_BUFFER_TOO_SMALL) {
        printf("输出缓冲区不足，需要大小: %zu 字节\n", size);
    } else if (ret == NMC_RESULT_ERROR_BUFFER_TOO_SMALL_ERROR) {
        printf("错误缓冲区不足，需要大小: %zu 字节\n", error_size);
    } else if (ret == NMC_RESULT_SUCCESS) {
        printf("输出: %s\n", output);
        printf("错误输出: %s\n", error);
    }
    
    /* Example 3: Connection list */
    printf("\n=== Example 3: Connection list ===\n");

    char *args3[] = {"nmcli", "connection", "show", "有线连接"};
    char output1[4096];
    size_t size1 = sizeof(output1);
    char error1[256];
    size_t error_size1 = sizeof(error1);
    char *dynamic_output = NULL;

    result = nmcli_execute_with_output(4, args3, output1, &size1, error1, &error_size1);

    if (result == NMC_RESULT_ERROR_BUFFER_TOO_SMALL) {
        printf("输出缓冲区不足，所需大小: %zu 字节，重新分配...\n", size1);
        dynamic_output = malloc(size1);
        result = nmcli_execute_with_output(4, args3, dynamic_output, &size1, error1, &error_size1);
        if (result == NMC_RESULT_SUCCESS) {
            printf("输出（重试）:\n%s\n", dynamic_output);
        } else {
            printf("重试调用失败，错误码: %d\n", result);
        }
    } else if (result == NMC_RESULT_ERROR_BUFFER_TOO_SMALL_ERROR) {
        printf("错误缓冲区不足，所需大小: %zu 字节\n", error_size1);
    } else if (result == NMC_RESULT_SUCCESS) {
        printf("输出:\n%s\n", output1);
    } else {
        printf("调用失败，错误码: %d\n", result);
    }

    if (strlen(error1) > 0) {
        printf("错误输出:\n%s\n", error1);
    }

    free(dynamic_output);

    printf("\nExample completed\n");
    return 0;
} 