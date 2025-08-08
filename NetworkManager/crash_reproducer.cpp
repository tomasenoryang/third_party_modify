#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <unistd.h>
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
        std::cout << "缓冲区不足，重新分配大小: " << output_size << std::endl;
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
    std::cout << "NetworkManager 崩溃复现程序" << std::endl;
    std::cout << "版本: " << nmcli_get_version() << std::endl << std::endl;

    const char *args[] = {"nmcli", "device", "status"};

    // 多次调用以触发崩溃
    for (int i = 0; i < 20; i++) {
        std::cout << "\n=== 第 " << (i + 1) << " 次调用 ===" << std::endl;
        
        try {
            std::cout << "调用 nmcli device status..." << std::endl;
            NmcliResult result = execute_nmcli_with_dynamic_buffer(3, const_cast<char**>(args));
            
            std::cout << "调用结果: " << result.exit_code << std::endl;
            std::cout << "调用成功: " << (result.success ? "是" : "否") << std::endl;
            std::cout << "输出长度: " << result.output.length() << std::endl;
            std::cout << "错误长度: " << result.error.length() << std::endl;
            
            if (!result.output.empty()) {
                std::cout << "输出前100字符: " << result.output.substr(0, 100) << std::endl;
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
        usleep(100000); // 100ms
    }
    
    std::cout << "\n崩溃复现测试完成" << std::endl;
    return 0;
}
