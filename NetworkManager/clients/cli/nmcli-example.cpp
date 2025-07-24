#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include "libnmcli.h"

/* 封装函数：执行nmcli命令并处理动态缓冲区调整 */
struct NmcliResult {
    bool success;
    std::string output;
    std::string error;
    int exit_code;
};

NmcliResult execute_nmcli_with_dynamic_buffer(int arg_count, char **args) {
    NmcliResult result;
    size_t output_size = 4096;
    size_t error_size = 256;
    
    std::vector<char> output_buffer(output_size);
    std::vector<char> error_buffer(error_size);
    
    int exit_code = nmcli_execute_with_output(arg_count, args, 
                                             output_buffer.data(), &output_size,
                                             error_buffer.data(), &error_size);
    
    // 处理缓冲区不足的情况
    if (exit_code == NMC_RESULT_ERROR_BUFFER_TOO_SMALL) {
        output_buffer.resize(output_size);
        error_buffer.resize(error_size);
        exit_code = nmcli_execute_with_output(arg_count, args,
                                             output_buffer.data(), &output_size,
                                             error_buffer.data(), &error_size);
    }
    
    result.success = (exit_code == NMC_RESULT_SUCCESS);
    result.output = std::string(output_buffer.data());
    result.error = std::string(error_buffer.data());
    result.exit_code = exit_code;
    
    return result;
}

int main(int argc, char **argv)
{
    std::cout << "nmcli Library Example (C++ Version)" << std::endl;
    std::cout << "Version: " << nmcli_get_version() << std::endl << std::endl;

    std::cout << "Library ready to use" << std::endl;

    /* Example 1: Simple command execution */
    std::cout << "\n=== Example 1: Simple command ===" << std::endl;
    const char *args1[] = {"nmcli", "device", "status"};
    int result = nmcli_execute(3, const_cast<char**>(args1));
    std::cout << "Result: " << result << std::endl;

    /* Example 2: Command with output capture using std::vector */
    std::cout << "\n=== Example 2: Command with output capture (std::vector) ===" << std::endl;
    const char *args2[] = {"nmcli", "device", "status"};
    
    /* 第一次调用 */
    std::cout << "第一次调用 nmcli device status:" << std::endl;
    NmcliResult result1 = execute_nmcli_with_dynamic_buffer(3, const_cast<char**>(args2));
    std::cout << "第一次调用结果: " << result1.exit_code << std::endl;
    if (result1.success) {
        std::cout << "第一次调用输出: " << result1.output << std::endl;
        std::cout << "第一次调用错误输出: " << result1.error << std::endl;
    } else {
        std::cout << "第一次调用失败: " << result1.error << std::endl;
    }
    
    /* 第二次调用 */
    std::cout << "\n第二次调用 nmcli device status:" << std::endl;
    NmcliResult result2 = execute_nmcli_with_dynamic_buffer(3, const_cast<char**>(args2));
    std::cout << "第二次调用结果: " << result2.exit_code << std::endl;
    if (result2.success) {
        std::cout << "第二次调用输出: " << result2.output << std::endl;
        std::cout << "第二次调用错误输出: " << result2.error << std::endl;
    } else {
        std::cout << "第二次调用失败: " << result2.error << std::endl;
    }
    
    /* Example 3: Connection list */
    std::cout << "\n=== Example 3: Connection list ===" << std::endl;
    const char *args3[] = {"nmcli", "connection", "show", "有线连接"};
    NmcliResult result3 = execute_nmcli_with_dynamic_buffer(4, const_cast<char**>(args3));
    
    if (result3.success) {
        std::cout << "输出:" << std::endl << result3.output << std::endl;
    } else {
        std::cout << "调用失败，错误码: " << result3.exit_code << std::endl;
    }
    
    if (!result3.error.empty()) {
        std::cout << "错误输出:" << std::endl << result3.error << std::endl;
    }

    /* Example 4: Final device status check */
    std::cout << "\n=== Example 4: Final device status check ===" << std::endl;
    
    // 设置环境变量以确保正确的输出格式
    // setenv("TERM", "xterm-256color", 1);
    // setenv("COLUMNS", "126", 1);
    
    // 现在全局状态应该被正确重置，使用标准命令
    std::cout << "执行最终命令: nmcli device status (全局状态已重置)" << std::endl;
    const char *args4[] = {"nmcli", "device", "status"};
    int final_result = nmcli_execute(3, const_cast<char**>(args4));
    std::cout << "最终执行结果: " << final_result << std::endl;

    std::cout << "\nExample completed" << std::endl;
    return 0;
} 