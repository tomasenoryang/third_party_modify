#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "libnmcli.h"

/* 封装函数：执行nmcli命令并处理动态缓冲区调整 */
typedef struct {
    int success;
    char *output;
    char *error;
    int exit_code;
} NmcliResult;

NmcliResult execute_nmcli_with_dynamic_buffer(int arg_count, char **args) {
    NmcliResult result = {0};
    size_t output_size = 4096;
    size_t error_size = 256;
    
    char *output_buffer = malloc(output_size);
    char *error_buffer = malloc(error_size);
    
    if (!output_buffer || !error_buffer) {
        result.success = 0;
        result.exit_code = -1;
        result.output = strdup("内存分配失败");
        result.error = strdup("");
        return result;
    }
    
    int exit_code = nmcli_execute_with_output(arg_count, args, 
                                             output_buffer, &output_size,
                                             error_buffer, &error_size);
    
    /* 处理缓冲区不足的情况 */
    if (exit_code == NMC_RESULT_ERROR_BUFFER_TOO_SMALL) {
        free(output_buffer);
        output_buffer = malloc(output_size);
        if (output_buffer) {
            exit_code = nmcli_execute_with_output(arg_count, args,
                                                 output_buffer, &output_size,
                                                 error_buffer, &error_size);
        }
    } else if (exit_code == NMC_RESULT_ERROR_BUFFER_TOO_SMALL_ERROR) {
        free(error_buffer);
        error_buffer = malloc(error_size);
        if (error_buffer) {
            exit_code = nmcli_execute_with_output(arg_count, args,
                                                 output_buffer, &output_size,
                                                 error_buffer, &error_size);
        }
    }
    
    result.success = (exit_code == NMC_RESULT_SUCCESS);
    result.output = strdup(output_buffer ? output_buffer : "");
    result.error = strdup(error_buffer ? error_buffer : "");
    result.exit_code = exit_code;
    
    free(output_buffer);
    free(error_buffer);
    
    return result;
}

/* 释放NmcliResult结构体的内存 */
void free_nmcli_result(NmcliResult *result) {
    if (result) {
        free(result->output);
        free(result->error);
        result->output = NULL;
        result->error = NULL;
    }
}

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

    /* Example 2: Command with output capture using encapsulated function */
    printf("\n=== Example 2: Command with output capture (encapsulated function) ===\n");
    char *args2[] = {"nmcli", "device", "status"};
    
    /* 第一次调用 */
    printf("第一次调用 nmcli device status:\n");
    NmcliResult result1 = execute_nmcli_with_dynamic_buffer(3, args2);
    printf("第一次调用结果: %d\n", result1.exit_code);
    if (result1.success) {
        printf("第一次调用输出: %s\n", result1.output);
        printf("第一次调用错误输出: %s\n", result1.error);
    } else {
        printf("第一次调用失败: %s\n", result1.error);
    }
    
    /* 第二次调用 */
    printf("\n第二次调用 nmcli device status:\n");
    NmcliResult result2 = execute_nmcli_with_dynamic_buffer(3, args2);
    printf("第二次调用结果: %d\n", result2.exit_code);
    if (result2.success) {
        printf("第二次调用输出: %s\n", result2.output);
        printf("第二次调用错误输出: %s\n", result2.error);
    } else {
        printf("第二次调用失败: %s\n", result2.error);
    }
    
    /* 清理内存 */
    free_nmcli_result(&result1);
    free_nmcli_result(&result2);
    
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