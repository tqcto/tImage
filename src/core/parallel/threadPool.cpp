#include "../../../include/tImage_definition.h"
#include "../../../include/core/parallel/threadPool.h"
#include "../../../include/core/cpu.h"

namespace tImage {
namespace core {
namespace parallel {

    threadPool::threadPool(void) {

        this->set_thread_num(t_CPU_INFO.num_cores);
        // this->set_thread_num(std::thread::hardware_concurrency());
        
    }
    // threadPool::threadPool(t_uint num_thread) {

    //     this->set_thread_num(num_thread);

    // }

    threadPool::~threadPool(void) {

        this->stop();
        /*
        {
            std::unique_lock<std::mutex> lock(this->queue_mutex);
            this->stop_flag = true;
        }

        this->condition.notify_all();

        for (std::thread& worker : this->workers) {
            worker.join();
        }
        */

    }

    void threadPool::set_thread_num(t_uint num_thread) {

        this->stop();
        this->stop_flag = false;
        this->workers.clear();

        for (t_int i = 0; i < static_cast<t_int>(num_thread); i++) {
            this->workers.emplace_back([this] {
                while(1) {

                    std::function<void()> task;
                    
                    {

                        std::unique_lock<std::mutex> lock(this->queue_mutex);

                        // wait come tasks
                        this->condition.wait(lock, [this] {
                            return this->stop_flag || !this->tasks.empty();
                        });

                        // if stood flag of stak and not exist tasks, then finish
                        if (this->stop_flag && this->tasks.empty()) {
                            return;
                        }

                        // pull task
                        task = std::move(this->tasks.front());
                        this->tasks.pop();

                    }

                    // run task
                    task();

                }
            });
        }

    }

    /*
    void threadPool::enqueue(std::function<void()> task) {

        {
            std::unique_lock<std::mutex> lock(this->queue_mutex);
            this->tasks.push(std::move((task)));
        }
        this->condition.notify_one();

    }
    */

}
}
}