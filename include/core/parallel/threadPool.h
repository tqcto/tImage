#pragma once
#include "../../../include/tImage_definition.h"

#include <thread>
#include <mutex>
#include <queue>
#include <functional>
#include <condition_variable>
#include <vector>
#include <future>
#include <memory>
#include <stdexcept>
#include <algorithm>
#include <future>

namespace tImage {
namespace core {
namespace parallel {

    class threadPool {
        
    private:

        std::vector<std::thread> workers;
        std::queue<std::function<void()>> tasks;
        
        std::mutex queue_mutex;
        std::condition_variable condition;

        // flag of stop
        t_bool stop_flag;

        inline void stop(void) {
            
            {
                std::unique_lock<std::mutex> lock(this->queue_mutex);
                this->stop_flag = true;
            }

            this->condition.notify_all();
            for (std::thread& worker : this->workers) {
                if (worker.joinable()) {
                    worker.join();
                }
            }
            
        }

    public:

        // auto set cores
        DLL_EXPORT threadPool(void);
        // set cores
        // DLL_EXPORT threadPool(t_uint num_thread);
        DLL_EXPORT ~threadPool(void);

        DLL_EXPORT void set_thread_num(t_uint num_thread);

        template<class F, class... Args>
        inline auto enqueue(F&& f, Args&&... args) 
        -> std::future<decltype(f(args...))> {

            using return_type = decltype(f(args...));
            
            auto task = std::make_shared<std::packaged_task<return_type()>>(
                std::bind(std::forward<F>(f), std::forward<Args>(args)...)
            );

            std::future<return_type> res = task->get_future();
            {

                std::unique_lock<std::mutex> lock(this->queue_mutex);
                if (this->stop_flag) {
                    throw std::runtime_error("enqueue on stopped threadPool");
                }
                this->tasks.emplace([task](){
                    (*task)();
                });

            }

            this->condition.notify_one();
            return res;

        }
        // DLL_EXPORT void enqueue(std::function<void()> task);

        // parallel forに類似したループ関数の実装
        template<typename Func>
        inline void pfor(t_int start, t_int end, Func&& func) {

            t_int total = end - start;
            t_int num_threads = static_cast<t_int>(this->workers.size());

            if (num_threads <= 1 || total <= 1) {
                for (t_int i = start; i < end; i++){
                    func(i);
                }
                return;
            }

            // auto split tasks
            t_int chunk_size = (total + num_threads - 1) / num_threads;
            std::vector<std::future<void>> futures;

            for (t_int i = 0; i < num_threads; i++) {

                t_int range_start = start + i * chunk_size;
                t_int range_end = (std::min)(range_start + chunk_size, end);

                if (range_start < range_end) {
                    futures.push_back(
                        this->enqueue([range_start, range_end, &func]() {
                            for (t_int i = range_start; i < range_end; i++) {
                                func(i);
                            }
                        })
                    );
                }

            }

            for (auto& f : futures) {
                f.wait();
            }

        }

        inline t_uint num_thread(void) const noexcept {
            return this->workers.size();
        }

    };

}
}
}
