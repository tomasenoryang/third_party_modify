#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <unistd.h>
#include <thread>
#include <chrono>
#include "libnmcli.h"

/* 封装函数：执行nmcli命令并处理动态缓冲区调整 */
struct NmcliResult {
    bool success;
    std::string output;
    std::string error;
    int exit_code;
};

NmcliResult executeNmcliCommand(const char* const* args, int arg_count) {
    NmcliResult result;
    
    // 初始缓冲区大小 - 设置为 10 字节，故意触发缓冲区不足
    size_t output_size = 10;  // 设置为 10 字节
    size_t error_size = 10;   // 设置为 10 字节
    
    std::vector<char> output_buffer(output_size);
    std::vector<char> error_buffer(error_size);
    
    std::cout << "第一次调用，缓冲区大小: " << output_size << std::endl;
    int exit_code = nmcli_execute_with_output(arg_count, const_cast<char**>(args), 
                                             output_buffer.data(), &output_size,
                                             error_buffer.data(), &error_size);
    
    std::cout << "第一次调用结果: " << exit_code << std::endl;
    
    // 处理缓冲区不足的情况
    if (exit_code == NMC_RESULT_ERROR_BUFFER_TOO_SMALL) {
        std::cout << "缓冲区不足，重新分配大小: " << output_size << std::endl;
        output_buffer.resize(output_size);
        error_buffer.resize(error_size);
        
        std::cout << "第二次调用 nmcli_execute_with_output..." << std::endl;
        exit_code = nmcli_execute_with_output(arg_count, const_cast<char**>(args),
                                             output_buffer.data(), &output_size,
                                             error_buffer.data(), &error_size);
        
        std::cout << "第二次调用结果: " << exit_code << std::endl;
    }
    
    result.success = (exit_code == NMC_RESULT_SUCCESS);
    result.output = std::string(output_buffer.data());
    result.error = std::string(error_buffer.data());
    result.exit_code = exit_code;
    
    return result;
}

int main(int argc, char **argv)
{
    std::cout << "NetworkManager 高级崩溃复现程序" << std::endl;
    std::cout << "版本: " << nmcli_get_version() << std::endl << std::endl;

    const char* args[] = {"nmcli", "device", "status"};

    // 多次调用以触发崩溃
    for (int i = 0; i < 50; i++) {
        std::cout << "\n=== 第 " << (i + 1) << " 次调用 ===" << std::endl;
        
        try {
            std::cout << "调用 executeNmcliCommand..." << std::endl;
            NmcliResult result = executeNmcliCommand(args, 3);
            
            std::cout << "调用结果: " << result.exit_code << std::endl;
            std::cout << "调用成功: " << (result.success ? "是" : "否") << std::endl;
            std::cout << "输出长度: " << result.output.length() << std::endl;
            std::cout << "错误长度: " << result.error.length() << std::endl;
            
            if (!result.output.empty()) {
                std::cout << "输出前50字符: " << result.output.substr(0, 50) << std::endl;
            }
            
            if (!result.error.empty()) {
                std::cout << "错误输出: " << result.error << std::endl;
            }
            
        } catch (const std::exception& e) {
            std::cerr << "异常: " << e.what() << std::endl;
            break;
        } catch (...) {
            std::cerr << "未知异常，可能发生了崩溃" << std::endl;
            break;
        }
        
        // 短暂延迟
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    
    std::cout << "\n高级崩溃复现测试完成" << std::endl;
    return 0;
}
