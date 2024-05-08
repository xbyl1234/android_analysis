/**
Copper v1.1
---

MIT License

Copyright (c) 2021 Andreas Tollkötter (github.com/atollk)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef INCLUDE_COPPER_H
#define INCLUDE_COPPER_H

#include <array>
#include <cassert>
#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <functional>
#include <mutex>
#include <numeric>
#include <optional>
#include <queue>
#include <random>
#include <limits>

#define COPPER_GET_MACRO2(_1, _2, NAME, ...) NAME
#define COPPER_STATIC_ASSERT(...) \
    COPPER_GET_MACRO2(__VA_ARGS__, COPPER_STATIC_ASSERT2, COPPER_STATIC_ASSERT1)(__VA_ARGS__)
#if __cpp_generic_lambdas >= 201707L
#define COPPER_STATIC_ASSERT1(condition)                   \
    []<bool flag = (condition)>() { static_assert(flag); } \
    ()
#define COPPER_STATIC_ASSERT2(condition, message)                   \
    []<bool flag = (condition)>() { static_assert(flag, message); } \
    ()
#else
#define COPPER_STATIC_ASSERT1(condition) ::copper::_detail::static_assert_ifc<condition>()
#define COPPER_STATIC_ASSERT2(condition, message) ::copper::_detail::static_assert_ifc<condition>()
#endif

namespace copper {

/** channel_op_status describes the success status of an operation on a channel object. */
    enum class channel_op_status {
        success,     /** The operation was completed successfully. */
        unavailable, /** The operation could not be completed due to resources being unavailable. */
        closed,      /** The operation could not be completed, as the channel object is already closed. */
    };

/** wait_type describes a strategy of waiting for an operation to finish. */
    enum class wait_type {
        forever, /** Wait for however long it takes to complete. */
        none,    /** Do not wait at all, if the operation cannot be completed immediately. */
        for_,    /** Wait for a set amount of time. */
        until,   /** Wait until a certain point in time. */
    };

    template<bool is_buffered,
            typename T,
            template<typename...> typename BufferQueue = std::queue,
            template<typename...> typename OpQueue = std::deque>
#if my__cpp_concepts >= 201907L
    requires(std::is_default_constructible_v<T>&& std::is_move_constructible_v<T>&& std::is_move_assignable_v<T>&&
                 std::is_destructible_v<T>) ||
        std::is_void_v<T>
#endif
    class channel;

    template<typename T, template<typename> typename... Args>
    using buffered_channel = channel<true, T, Args...>;

    template<typename T, template<typename> typename... Args>
    using chan = buffered_channel<T, Args...>;

//    template<typename T, template<typename> typename... Args>
//    using chan = std::shared_ptr<buffered_channel<T, Args...>>;

//    template<typename T, template<typename> typename... Args>
//    inline auto make_shared(int maxSize) {
//        return std::make_shared<buffered_channel<T, Args...>>(maxSize);
//    }
//
//    template<typename T, template<typename> typename... Args>
//    inline auto make_shared() {
//        return std::make_shared<buffered_channel<T, Args...>>();
//    }

    template<typename T, template<typename> typename... Args>
    using unbuffered_channel = channel<false, T, Args...>;

    namespace _detail {

#if __cpp_generic_lambdas < 201707L

        template <bool condition>
        void static_assert_ifc() {
            static_assert(condition);
        }

#endif

// The use of std::recursive_mutex can be replaced by std::mutex with a macro definition.
// This can cause deadlocks when using callback functions though.
#ifdef COPPER_DISALLOW_MUTEX_RECURSION
        using channel_mutex_t = std::mutex;
        using channel_cv_t = std::condition_variable;
#else
        using channel_mutex_t = std::recursive_mutex;
        using channel_cv_t = std::condition_variable_any;
#endif

        template<typename... Ops>
        struct select_manager;

        template<bool is_buffered, typename T, template<typename...> typename... Args>
        struct popper_base;

        template<typename F, bool is_buffered, typename T, template<typename...> typename... Args>
        struct popper;

        template<bool is_buffered, typename T, template<typename...> typename... Args>
        struct pusher_base;

        template<typename F, bool is_buffered, typename T, template<typename...> typename... Args>
        struct pusher;

/** Helper class to pass a mutex "lock" to a function when nothing is actually locked. */
        struct dummy_lock_t final {
            constexpr void lock() const {}

            constexpr void unlock() const {}
        };

        constexpr auto dummy_lock = dummy_lock_t();

        template<typename F, typename... Ts>
        void visit_at(const std::tuple<Ts...> &tup, size_t idx, F &&fun);

        template<typename F, typename... Ts>
        void visit_at(std::tuple<Ts...> &tup, size_t idx, F &&fun);

        template<typename... Ts, typename F>
        void for_each_in_tuple(const std::tuple<Ts...> &t, F &&f);

        template<size_t N>
        static void fill_with_random_indices(std::array<size_t, N> &indices);

        template<typename... Args>
        struct pairify_template {
        };

        template<typename... Ts, typename F>
        auto transform_tuple(const std::tuple<Ts...> &t, F &&f);

    }  // namespace _detail

    template<typename ChannelT>
    class channel_read_view;

    template<typename ChannelT>
    class channel_write_view;

    template<typename ChannelT>
    class channel_pop_iterator;

    template<typename ChannelT>
    class channel_push_iterator;

    namespace _detail {

/** The internal buffer used to store elements within a channel. Has some overloads for void and unbuffered channels. */
        template<bool is_buffered, typename T, template<typename...> typename BufferQueue>
        struct channel_buffer;

/** The internal buffer used to store elements within a channel. Has some overloads for void and unbuffered channels. */
        template<typename T, template<typename...> typename BufferQueue>
        struct channel_buffer<false, T, BufferQueue> {
            [[nodiscard]] constexpr bool is_empty() { return true; }

            [[nodiscard]] constexpr bool is_full() { return true; }

            [[nodiscard]] constexpr T pop() {
                if constexpr (!std::is_void_v<T>) {
                    return T();
                }
            }

            template<typename U>
            constexpr void push(U &&) {}

            constexpr void push() {}

            [[nodiscard]] constexpr size_t clear() { return 0; }
        };

/** Helper class for channel_buffer. */
        template<typename T, typename = void>
        struct has_clear : std::false_type {
        };

/** Helper class for channel_buffer. */
        template<typename T>
        struct has_clear<T, decltype(void(std::declval<T &>().clear()))> : std::true_type {
        };

/** The internal buffer used to store elements within a channel. Has some overloads for void and unbuffered channels. */
        template<typename T, template<typename...> typename BufferQueue>
        struct channel_buffer<true, T, BufferQueue> {
            explicit channel_buffer(size_t max_size = (std::numeric_limits<size_t>::max)())
                    : _max_size(max_size) {}

            [[nodiscard]] bool is_empty() const { return this->_buffer.empty(); }

            [[nodiscard]] bool is_full() const { return this->_buffer.size() >= this->_max_size; }

            [[nodiscard]] size_t clear() {
                const auto tmp = this->_buffer.size();
                if constexpr (has_clear<decltype(this->_buffer)>::value) {
                    this->_buffer.clear();
                } else {
                    while (!this->_buffer.empty()) {
                        this->_buffer.pop();
                    }
                }
                return tmp;
            }

            [[nodiscard]] T pop() {
                auto tmp = std::move(this->_buffer.front());
                this->_buffer.pop();
                return tmp;
            }

            void push(T &&t) { this->_buffer.push(std::move(t)); }

        private:
            size_t _max_size;
            BufferQueue<T> _buffer;
        };

/** The internal buffer used to store elements within a channel. Has some overloads for void and unbuffered channels. */
        template<template<typename...> typename BufferQueue>
        struct channel_buffer<true, void, BufferQueue> {
            explicit channel_buffer(size_t max_size = (std::numeric_limits<size_t>::max)())
                    : _max_size(max_size), _buffer(0) {}

            [[nodiscard]] bool is_empty() const { return this->_buffer == 0; }

            [[nodiscard]] bool is_full() const { return this->_buffer >= this->_max_size; }

            [[nodiscard]] size_t clear() {
                const auto tmp = this->_buffer;
                this->_buffer = 0;
                return tmp;
            }

            void pop() { this->_buffer -= 1; }

            void push() { this->_buffer += 1; }

        private:
            size_t _max_size;
            size_t _buffer;
        };

/** Template argument deduction for the constructor of popper. */
        template<typename F, bool is_buffered, typename T, template<typename...> typename... Args>
        auto make_popper(channel<is_buffered, T, Args...> &channel, F &&func) {
            return popper<F, is_buffered, T, Args...>(channel, std::forward<F>(func));
        }

/** Template argument deduction for the constructor of pusher. */
        template<typename F, bool is_buffered, typename T, template<typename...> typename... Args>
        auto make_pusher(channel<is_buffered, T, Args...> &channel, F &&func) {
            return pusher<F, is_buffered, T, Args...>(channel, std::forward<F>(func));
        }

/** Base class for specific waiting_op_group types that is used within popper / pusher objects. */
        struct waiting_op_group_base {
            /** Tries to find a "match" for this group by using the given pusher and given popper.
             * If a match was already found, `false` is returned instead and nothing is done.
             * If this is the new match, both objects have their functors called. */
            template<bool is_buffered, typename T, template<typename...> typename... Args>
            bool match(pusher_base<is_buffered, T, Args...> &pusher,
                       popper_base<is_buffered, T, Args...> &popper) {
                assert(pusher._channel == popper._channel);

                auto lock = std::unique_lock<channel_mutex_t>(this->mutex);
                if (this->_match_found) {
                    return false;
                }
                this->_match_found = true;
                if constexpr (std::is_void_v<T>) {
                    pusher();
                    popper();
                } else {
                    popper(pusher());
                }
                lock.unlock();
                this->cv.notify_all();
                return true;
            }

            channel_mutex_t mutex;
            channel_cv_t cv;
            int open_op_channels = 0;

        protected:
            bool _match_found = false;
        };

/** Represents a disjunction of pending operations on a channel object.
 * Exactly one of the included operations is supposed to be executed.
 * The class makes sure that at most one will be executed. */
        template<typename... Ops>
        struct waiting_op_group final : public waiting_op_group_base {
            explicit waiting_op_group(Ops &... ops) : ops(&ops...) {}

            waiting_op_group(const waiting_op_group &) = delete;

            waiting_op_group(waiting_op_group &&) = delete;

            waiting_op_group &operator=(const waiting_op_group &) = delete;

            waiting_op_group &operator=(waiting_op_group &&) = delete;

            ~waiting_op_group() {
                if (this->_active) {
                    _detail::for_each_in_tuple(this->ops,
                                               [](size_t, auto *op) { op->parent_destroyed(); });
                }
            }

            /** Notifies all relevant channels of all contained operations, so that when `wait` is called, it
             * actually gets notified by changes. */
            void activate() {
                auto lock = std::unique_lock<channel_mutex_t>(this->mutex);
                this->_active = true;
                const auto initialize_op = [this](size_t, auto *op) {
                    op->set_op_group(this);
                    if (op->channel_open()) {
                        this->open_op_channels += 1;
                        op->start_waiting();
                    }
                };
                _detail::for_each_in_tuple(this->ops, initialize_op);
            }

            /** Waits until a match is found or the channels of all operations have been closed using a given waiting strategy.
             * Returns `true` if a match was found and `false` otherwise. */
            template<wait_type wtype, typename... Args>
            [[nodiscard]] channel_op_status wait(Args &&... args) {
                static_assert(wtype != wait_type::none);
                auto lock = std::unique_lock<channel_mutex_t>(this->mutex);
                const auto f = [this]() {
                    return this->_match_found || this->open_op_channels == 0;
                };
                if constexpr (wtype == wait_type::forever) {
                    COPPER_STATIC_ASSERT(sizeof...(Args) == 0);
                    this->cv.wait(lock, std::forward<Args>(args)..., f);
                } else if constexpr (wtype == wait_type::for_) {
                    COPPER_STATIC_ASSERT(sizeof...(Args) == 1);
                    this->cv.wait_for(lock, std::forward<Args>(args)..., f);
                } else if constexpr (wtype == wait_type::until) {
                    COPPER_STATIC_ASSERT(sizeof...(Args) == 1);
                    this->cv.wait_until(lock, std::forward<Args>(args)..., f);
                }
                if (this->_match_found) {
                    return channel_op_status::success;
                } else if (this->open_op_channels == 0) {
                    return channel_op_status::closed;
                } else {
                    return channel_op_status::unavailable;
                }
            }

            std::tuple<Ops *...> ops;

        private:
            bool _active = false;
        };

/** Template argument deduction for the constructor of waiting_op_group. */
        template<typename... Ops>
        waiting_op_group<Ops...> make_waiting_op_group(Ops &... ops) {
            return waiting_op_group<Ops...>(ops...);
        }

    }  // namespace _detail

/** The main class to represent channels.
 * @tparam is_buffered If `true`, the channel contains an internal buffer to store some objects for use.
 * @tparam T The type of the objects the channel works on.
 * @tparam BufferQueue The type of a queue-like class used for the internal buffer. Not used it `is_buffered` is `false`
 * or `T` is `void`.
 * @tparam OpQueue The type of a deque-like class used for storing actively waiting push/pop operations.
 */
    template<bool is_buffered,
            typename T,
            template<typename...>
            typename BufferQueue,
            template<typename...>
            typename OpQueue>
#if my__cpp_concepts >= 201907L
    requires(std::is_default_constructible_v<T>&& std::is_move_constructible_v<T>&& std::is_move_assignable_v<T>&&
                 std::is_destructible_v<T>) ||
        std::is_void_v<T>
#endif
    class channel {
    public:
        using value_type = T;

        channel() = default;

        explicit channel(size_t max_buffer_size)
#if my__cpp_concepts >= 201907L
        requires(is_buffered)
#endif
                : _buffer(max_buffer_size) {
        }

        channel(const channel &other) = delete;

        channel(channel &&other) noexcept = default;

        channel &operator=(const channel &) = delete;

        channel &operator=(channel &&) = delete;

        ~channel() {
            assert(this->_waiting_pushes.empty());
            assert(this->_waiting_pops.empty());
        }

        template<wait_type wtype, typename T_ = T, typename... Args>
        [[nodiscard]] std::enable_if_t<!std::is_void_v<T_>, std::optional<T>>
        pop_wt(Args &&... args) {
            static_assert(!std::is_void_v<T>);
            auto result = T();
            auto f = [&result](T &&data) { result = std::move(data); };
            const auto op_status = this->pop_func_wt<wtype>(std::move(f),
                                                            std::forward<Args>(args)...);
            if (op_status == channel_op_status::success) {
                return result;
            } else {
                return std::nullopt;
            }
        }

        template<wait_type wtype, typename T_ = T, typename... Args>
        [[nodiscard]] std::enable_if_t<std::is_void_v<T_>, bool> pop_wt(Args &&... args) {
            static_assert(std::is_void_v<T>);
            const auto op_status = this->pop_func_wt<wtype>([] {}, std::forward<Args>(args)...);
            return op_status == channel_op_status::success;
        }

        template<wait_type wtype, typename F, typename... Args>
#if my__cpp_concepts >= 201907L
        requires requires(F f) {
            f(std::declval<value_type>());
        } ||(requires(F f) { f(); } && std::is_void_v<T>)
#endif
        [[nodiscard]] channel_op_status pop_func_wt(F &&func, Args &&... args) {
            auto lock = std::unique_lock<_detail::channel_mutex_t>(_mutex);
            return this->_pop_func_with_lock_general<wtype>(std::forward<F>(func), lock,
                                                            std::forward<Args>(args)...);
        }

        [[nodiscard]] std::conditional_t<std::is_void_v<T>, bool, std::optional<T>> pop() {
            return this->pop_wt<wait_type::forever>();
        }

        [[nodiscard]] std::conditional_t<std::is_void_v<T>, bool, std::optional<T>> try_pop() {
            return this->pop_wt<wait_type::none>();
        }

        template<typename Rep, typename Period>
        [[nodiscard]] std::conditional_t<std::is_void_v<T>, bool, std::optional<T>> try_pop_for(
                const std::chrono::duration<Rep, Period> &rel_time) {
            return this->pop_wt<wait_type::for_>(rel_time);
        }

        template<typename Clock, typename Duration>
        [[nodiscard]] std::conditional_t<std::is_void_v<T>, bool, std::optional<T>> try_pop_until(
                const std::chrono::time_point<Clock, Duration> &timeout_time) {
            return this->pop_wt<wait_type::until>(timeout_time);
        }

        template<typename F>
        [[nodiscard]] channel_op_status pop_func(F &&func) {
            return this->pop_func_wt<wait_type::forever>(std::forward<F>(func));
        }

        template<typename F>
        [[nodiscard]] channel_op_status try_pop_func(F &&func) {
            return this->pop_func_wt<wait_type::none>(std::forward<F>(func));
        }

        template<typename F, typename Rep, typename Period>
        [[nodiscard]] channel_op_status
        try_pop_func_for(F &&func, const std::chrono::duration<Rep, Period> &rel_time) {
            return this->pop_func_wt<wait_type::for_>(std::forward<F>(func), rel_time);
        }

        template<typename F, typename Clock, typename Duration>
        [[nodiscard]] channel_op_status try_pop_func_until(F &&func,
                                                           const std::chrono::time_point<Clock, Duration> &timeout_time) {
            return this->pop_func_wt<wait_type::until>(std::forward<F>(func), timeout_time);
        }

        template<wait_type wtype,
                typename U,
                typename T_ = T,
                typename std::enable_if_t<!std::is_void_v<T_>, int> = 0,
                typename... Args>
#if my__cpp_concepts >= 201907L
        requires std::convertible_to<U, value_type>
#endif
        [[nodiscard]] bool push_wt(U &&data, Args &&... args) {
            static_assert(!std::is_void_v<T>);
            const auto op_status =
                    this->push_func_wt<wtype>([&data]() { return std::forward<U>(data); },
                                              std::forward<Args>(args)...);
            return op_status == channel_op_status::success;
        }

        template<wait_type wtype,
                typename U = T,
                typename T_ = T,
                typename std::enable_if_t<std::is_void_v<T_>, int> = 0,
                typename... Args>
        [[nodiscard]] bool push_wt(Args &&... args) {
            static_assert(std::is_void_v<T>);
            const auto op_status = this->push_func_wt<wtype>([] {}, std::forward<Args>(args)...);
            return op_status == channel_op_status::success;
        }

        template<wait_type wtype, typename F, typename... Args>
#if my__cpp_concepts >= 201907L
        requires requires(F f) {
            { f() } -> std::convertible_to<value_type>;
        }
#endif
        [[nodiscard]] channel_op_status push_func_wt(F &&func, Args &&... args) {
            auto lock = std::unique_lock<_detail::channel_mutex_t>(_mutex);
            return this->_push_func_with_lock_general<wtype>(std::forward<F>(func), lock,
                                                             std::forward<Args>(args)...);
        }

        template<typename... Args>
        [[nodiscard]] bool push(Args &&... args) {
            return this->push_wt<wait_type::forever>(std::forward<Args>(args)...);
        }

        template<typename... Args>
        [[nodiscard]] bool try_push(Args &&... args) {
            return this->push_wt<wait_type::none>(std::forward<Args>(args)...);
        }

        template<typename... Args>
        [[nodiscard]] bool try_push_for(Args &&... args) {
            return this->push_wt<wait_type::for_>(std::forward<Args>(args)...);
        }

        template<typename... Args>
        [[nodiscard]] bool try_push_until(Args &&... args) {
            return this->push_wt<wait_type::until>(std::forward<Args>(args)...);
        }

        template<typename F>
        [[nodiscard]] channel_op_status push_func(F &&func) {
            return this->push_func_wt<wait_type::forever>(std::forward<F>(func));
        }

        template<typename F>
        [[nodiscard]] channel_op_status try_push_func(F &&func) {
            return this->push_func_wt<wait_type::none>(std::forward<F>(func));
        }

        template<typename F, typename Rep, typename Period>
        [[nodiscard]] channel_op_status
        try_push_func_for(F &&func, const std::chrono::duration<Rep, Period> &rel_time) {
            return this->push_func_wt<wait_type::for_>(std::forward<F>(func), rel_time);
        }

        template<typename F, typename Clock, typename Duration>
        [[nodiscard]] channel_op_status try_push_func_until(F &&func,
                                                            const std::chrono::time_point<Clock, Duration> &timeout_time) {
            return this->push_func_wt<wait_type::until>(std::forward<F>(func), timeout_time);
        }

        [[nodiscard]] channel_pop_iterator<channel> begin() {
            return channel_pop_iterator<channel>(*this);
        }

        [[nodiscard]] channel_pop_iterator<channel>
        end() { return channel_pop_iterator<channel>(); }

        [[nodiscard]] channel_push_iterator<channel>
        push_iterator() { return channel_push_iterator<channel>(*this); }

        [[nodiscard]] channel_read_view<channel> read_view() {
            return channel_read_view<channel>(*this);
        }

        [[nodiscard]] channel_write_view<channel> write_view() {
            return channel_write_view<channel>(*this);
        }

        /** Clears only(!) the buffer, not pending push operations. */
        size_t clear() {
            const auto lock = std::lock_guard<_detail::channel_mutex_t>(this->_mutex);
            const auto size = this->_buffer.clear();;

            // If there are any "push" operations pending, execute the next one to re-fill the buffer.
            if (this->_open && !this->_waiting_pushes.empty()) {
                if constexpr (std::is_void_v<T>) {
                    auto tmp_op = _detail::make_popper(*this, [] {});
                    while (!this->_buffer.is_full() && !this->_waiting_pushes.empty()) {
                        const auto next_push = this->_waiting_pushes.front();
                        this->_waiting_pushes.pop_front();
                        if (next_push->match(tmp_op)) {
                            this->_buffer.push();
                        }
                    }
                } else {
                    auto tmp = T();
                    auto tmp_op = _detail::make_popper(*this,
                                                       [&tmp](T &&x) { tmp = std::move(x); });
                    while (!this->_buffer.is_full() && !this->_waiting_pushes.empty()) {
                        const auto next_push = this->_waiting_pushes.front();
                        this->_waiting_pushes.pop_front();
                        if (next_push->match(tmp_op)) {
                            this->_buffer.push(std::move(tmp));
                            break;
                        }
                    }
                }
            }

            return size;
        }

        void close() {
            const auto lock = std::lock_guard<_detail::channel_mutex_t>(this->_mutex);
            if (this->_open) {
                this->_open = false;
                for (auto *popper: this->_waiting_pops) {
                    popper->channel_closed();
                }
                this->_waiting_pops.clear();
                for (auto *pusher: this->_waiting_pushes) {
                    pusher->channel_closed();
                }
                this->_waiting_pushes.clear();
            }
        }

        [[nodiscard]] bool is_read_closed() {
            const auto lock = std::lock_guard<_detail::channel_mutex_t>(this->_mutex);
            return !this->_open && this->_buffer.is_empty();
        }

        [[nodiscard]] bool is_write_closed() {
            const auto lock = std::lock_guard<_detail::channel_mutex_t>(this->_mutex);
            return !this->_open;
        }

    private:
        using popper_t = _detail::popper_base<is_buffered, T, BufferQueue, OpQueue>;
        using pusher_t = _detail::pusher_base<is_buffered, T, BufferQueue, OpQueue>;

        friend popper_t;
        friend pusher_t;

        bool _open = true;
        _detail::channel_buffer<is_buffered, T, BufferQueue> _buffer{};
        _detail::channel_mutex_t _mutex;
        OpQueue<popper_t *> _waiting_pops{};
        OpQueue<pusher_t *> _waiting_pushes{};

        /**
         * The most general "pop" function of a channel.
         *
         * Tries to remove the next element from the represented queue and calls a functor with it.
         *
         * @tparam wtype The waiting strategy to deploy when the operation cannot be completed immediately.
         * @param op The operation to perform. If it is not of popper_t, it is converted into one.
         * @param lock The lock which guards the access to the channel's mutex. Is unlocked inbetween for performance gain.
         * @param args Any additional arguments to the waiting process.
         * @return The success status of the operation.
         */
        template<wait_type wtype, typename Op, typename Lock, typename... Args>
        [[nodiscard]] channel_op_status
        _pop_func_with_lock_general(Op &&op, Lock &lock, Args &&... args) {
            // Convert `op` into a "popper" if necessary.
            if constexpr (!std::is_base_of_v<popper_t, std::decay_t<Op>>) {
                auto popper = _detail::make_popper(*this, std::forward<Op>(op));
                return _pop_func_with_lock_general<wtype>(popper, lock,
                                                          std::forward<Args>(args)...);
            } else {
                // If the channel is closed, nothing can be done.
                if (!this->_open && this->_buffer.is_empty()) {
                    return channel_op_status::closed;
                }

                // The first source of elements is the buffer.
                if (!this->_buffer.is_empty()) {
                    // Take the next element from the buffer and run the callback.
                    if constexpr (std::is_void_v<T>) {
                        this->_buffer.pop();
                        op();
                    } else {
                        op(this->_buffer.pop());
                    }

                    // If there are any "push" operations pending, execute the next one to re-fill the buffer.
                    if (this->_open && !this->_waiting_pushes.empty()) {
                        if constexpr (std::is_void_v<T>) {
                            auto tmp_op = _detail::make_popper(*this, [] {});
                            while (!this->_waiting_pushes.empty()) {
                                const auto next_push = this->_waiting_pushes.front();
                                this->_waiting_pushes.pop_front();
                                if (next_push->match(tmp_op)) {
                                    this->_buffer.push();
                                    break;
                                }
                            }
                        } else {
                            auto tmp = T();
                            auto tmp_op = _detail::make_popper(*this, [&tmp](T &&x) {
                                tmp = std::move(x);
                            });
                            while (!this->_waiting_pushes.empty()) {
                                const auto next_push = this->_waiting_pushes.front();
                                this->_waiting_pushes.pop_front();
                                if (next_push->match(tmp_op)) {
                                    this->_buffer.push(std::move(tmp));
                                    break;
                                }
                            }
                        }
                    }
                    return channel_op_status::success;
                }

                // The second source of elements are pending "push" operations. If any is available, execute that now.
                while (!this->_waiting_pushes.empty()) {
                    const auto next_push = this->_waiting_pushes.front();
                    this->_waiting_pushes.pop_front();
                    if (next_push->match(op)) {
                        return channel_op_status::success;
                    }
                }

                // If neither source had an element, the channel has to wait for a new one to arrive.
                if constexpr (wtype == wait_type::none) {
                    return channel_op_status::unavailable;
                } else {
                    auto g = _detail::make_waiting_op_group(op);
                    g.activate();
                    lock.unlock();
                    return g.template wait<wtype>(std::forward<Args>(args)...);
                }
            }
        }

        /**
         * The most general "push" function of a channel.
         *
         * Tries to insert a new element into the represented queue, which is defined by the return value of a functor.
         *
         * @tparam wtype The waiting strategy to deploy when the operation cannot be completed immediately.
         * @param op The operation to perform. If it is not of pusher_t, it is converted into one.
         * @param lock The lock which guards the access to the channel's mutex. Is unlocked inbetween for performance gain.
         * @param args Any additional arguments to the waiting process.
         * @return The success status of the operation.
         */
        template<wait_type wtype, typename Op, typename Lock, typename... Args>
        [[nodiscard]] channel_op_status
        _push_func_with_lock_general(Op &&op, Lock &lock, Args &&... args) {
            // Convert `op` into a "pusher" if necessary.
            if constexpr (!std::is_base_of_v<pusher_t, std::decay_t<Op>>) {
                auto pusher = _detail::make_pusher(*this, std::forward<Op>(op));
                return _push_func_with_lock_general<wtype>(pusher, lock,
                                                           std::forward<Args>(args)...);
            } else {
                // If the channel is closed, nothing can be done.
                if (!this->_open) {
                    return channel_op_status::closed;
                }

                // The first sink of elements are pending "pop" operations. If any is available, execute that now.
                if (!this->_waiting_pops.empty()) {
                    while (!this->_waiting_pops.empty()) {
                        const auto next_pop = this->_waiting_pops.front();
                        this->_waiting_pops.pop_front();
                        if (next_pop->match(op)) {
                            return channel_op_status::success;
                        }
                    }
                }

                // The second sink of elements is the buffer. Use that, if it is not full already.
                if (!this->_buffer.is_full()) {
                    if constexpr (std::is_void_v<T>) {
                        op();
                        this->_buffer.push();
                    } else {
                        this->_buffer.push(op());
                    }
                    return channel_op_status::success;
                }

                // If neither sink could be used, the channel has to wait for a new one to arrive.
                if constexpr (wtype == wait_type::none) {
                    return channel_op_status::unavailable;
                } else {
                    auto g = _detail::make_waiting_op_group(op);
                    g.activate();
                    lock.unlock();
                    return g.template wait<wtype>(std::forward<Args>(args)...);
                }
            }
        }
    };

    namespace _detail {

/** Helper base class for `popper_base` for non-void types. */
        template<typename T>
        struct popper_base_nonvoid {
            virtual void operator()(T &&) = 0;
        };

/** Helper base class for `popper_base` for `void`. */
        struct popper_base_void {
            virtual void operator()() = 0;
        };

/** Abstract base class of `popper` to remove the dependency on a specific callable type.
 * Most of the logic is implemented here though.
 * This class represents a "pop" operation on a channel. */
        template<bool is_buffered, typename T, template<typename...> typename... Args>
        struct popper_base
                : std::conditional_t<std::is_void_v<T>, popper_base_void, popper_base_nonvoid<T>> {
            using channel_t = channel<is_buffered, T, Args...>;

            static constexpr bool is_op_base = true;

            explicit popper_base(channel_t &channel) : _channel(&channel) {}

            /** Notifies the respective channel that this operation is now waiting on an update. */
            void start_waiting() {
                this->_channel->_waiting_pops.push_back(this);
                this->_active = true;
            }

            /** Called when the parent `waiting_op_group` is destroyed so that this operation knows to stop waiting on
             * events. */
            void parent_destroyed() {
                const auto lock = std::lock_guard(this->_channel->_mutex);
#if __cpp_lib_erase_if >= 202002L
                std::erase(this->_channel->_waiting_pops, this);
#else
                this->_channel->_waiting_pops.erase(
                        std::remove(this->_channel->_waiting_pops.begin(),
                                    this->_channel->_waiting_pops.end(), this),
                        this->_channel->_waiting_pops.end());
#endif
                this->_active = false;
            }

            /** Called when the respective channel is closed so that this operation knows to stop waiting on events and to
             * pass this information to its parent `waiting_op_group`. */
            void channel_closed() {
                const auto lock = std::lock_guard(this->_parent->mutex);
//#if __cpp_lib_erase_if >= 202002L
//                std::erase(this->_channel->_waiting_pops, this);
//#else
//                this->_channel->_waiting_pops.erase(
//                        std::remove(this->_channel->_waiting_pops.begin(),
//                                    this->_channel->_waiting_pops.end(), this),
//                        this->_channel->_waiting_pops.end());
//#endif
                this->_active = false;
                this->_parent->open_op_channels -= 1;
                this->_parent->cv.notify_all();
            }

            bool channel_open() { return this->_channel->_open; }

            bool match(pusher_base<is_buffered, T, Args...> &other) {
                return (this->_parent->match(other, *this));
            }

            void set_op_group(waiting_op_group_base *parent) { this->_parent = parent; }

            template<wait_type wtype, typename... Brgs>
            [[nodiscard]] channel_op_status select(Brgs &&... brgs) {
                return this->_channel->template pop_func_wt<wtype>(*this,
                                                                   std::forward<Brgs>(brgs)...);
            }

            [[nodiscard]] channel_op_status
            try_select() { return this->template select<wait_type::none>(); }

            [[nodiscard]] channel_op_status try_select_locked() {
                return _channel->template _pop_func_with_lock_general<wait_type::none>(*this,
                                                                                       dummy_lock);
            }

            [[nodiscard]] channel_mutex_t &channel_mutex() const { return this->_channel->_mutex; }

        private:
            friend struct waiting_op_group_base;

            bool _active = false;
            channel_t *_channel;
            waiting_op_group_base *_parent = nullptr;
        };

/** Subclass of `popper_base` for a specific callback type and non-void channel elements. */
        template<typename F, bool is_buffered, typename T, template<typename...> typename... Args>
        struct popper final : public popper_base<is_buffered, T, Args...> {
            using base_t = popper_base<is_buffered, T, Args...>;
            using channel_t = typename base_t::channel_t;

            template<typename G>
            explicit
            popper(channel_t &channel, G &&func) : base_t(channel), func(std::forward<G>(func)) {}

            void operator()(T &&t) final { std::invoke(this->func, std::move(t)); }

            F func;
        };

/** Subclass of `popper_base` for a specific callback type and void channel elements. */
        template<typename F, bool is_buffered, template<typename...> typename... Args>
        struct popper<F, is_buffered, void, Args...> final
                : public popper_base<is_buffered, void, Args...> {
            using base_t = popper_base<is_buffered, void, Args...>;
            using channel_t = typename base_t::channel_t;

            template<typename G>
            explicit
            popper(channel_t &channel, G &&func) : base_t(channel), func(std::forward<G>(func)) {}

            void operator()() final { std::invoke(this->func); }

            F func;
        };

/** Abstract base class of `pusher` to remove the dependency on a specific callable type.
 * Most of the logic is implemented here though.
 * This class represents a "push" operation on a channel. */
        template<bool is_buffered, typename T, template<typename...> typename... Args>
        struct pusher_base {
            using channel_t = channel<is_buffered, T, Args...>;

            static constexpr bool is_op_base = true;

            explicit pusher_base(channel_t &channel) : _channel(&channel) {}

            virtual T operator()() = 0;

            /** Notifies the respective channel that this operation is now waiting on an update. */
            void start_waiting() {
                this->_channel->_waiting_pushes.push_back(this);
                this->_active = true;
            }

            /** Called when the parent `waiting_op_group` is destroyed so that this operation knows to stop waiting on
             * events. */
            void parent_destroyed() {
                const auto lock = std::lock_guard(this->_channel->_mutex);
#if __cpp_lib_erase_if >= 202002L
                std::erase(this->_channel->_waiting_pushes, this);
#else
                this->_channel->_waiting_pushes.erase(
                        std::remove(this->_channel->_waiting_pushes.begin(),
                                    this->_channel->_waiting_pushes.end(), this),
                        this->_channel->_waiting_pushes.end());
#endif
                this->_active = false;
            }

            /** Called when the respective channel is closed so that this operation knows to stop waiting on events and to
             * pass this information to its parent `waiting_op_group`. */
            void channel_closed() {
                const auto lock = std::lock_guard(this->_parent->mutex);
//#if __cpp_lib_erase_if >= 202002L
//                std::erase(this->_channel->_waiting_pushes, this);
//#else
//                this->_channel->_waiting_pushes.erase(
//                        std::remove(this->_channel->_waiting_pushes.begin(),
//                                    this->_channel->_waiting_pushes.end(), this),
//                        this->_channel->_waiting_pushes.end());
//#endif
                this->_active = false;
                this->_parent->open_op_channels -= 1;
                this->_parent->cv.notify_all();
            }

            bool channel_open() { return this->_channel->_open; }

            bool match(popper_base<is_buffered, T, Args...> &other) {
                return this->_parent->match(*this, other);
            }

            void set_op_group(waiting_op_group_base *parent) { this->_parent = parent; }

            template<wait_type wtype, typename... Brgs>
            [[nodiscard]] channel_op_status select(Brgs &&... brgs) {
                return this->_channel->template push_func_wt<wtype>(*this,
                                                                    std::forward<Brgs>(brgs)...);
            }

            [[nodiscard]] channel_op_status
            try_select() { return this->template select<wait_type::none>(); }

            [[nodiscard]] channel_op_status try_select_locked() {
                return this->_channel->template _push_func_with_lock_general<wait_type::none>(*this,
                                                                                              dummy_lock);
            }

            [[nodiscard]] channel_mutex_t &channel_mutex() const { return this->_channel->_mutex; }

        private:
            friend struct waiting_op_group_base;

            bool _active = false;
            channel_t *_channel;
            waiting_op_group_base *_parent = nullptr;
        };

/** Subclass of `pusher_base` for a specific callback type and non-void channel elements. */
        template<typename F, bool is_buffered, typename T, template<typename...> typename... Args>
        struct pusher final : public pusher_base<is_buffered, T, Args...> {
            using base_t = pusher_base<is_buffered, T, Args...>;
            using channel_t = typename base_t::channel_t;

            template<typename G>
            explicit
            pusher(channel_t &channel, G &&func) : base_t(channel), func(std::forward<G>(func)) {}

            T operator()() final { return std::invoke(this->func); }

            F func;
        };

/** Subclass of `pusher_base` for a specific callback type and non-void channel elements. */
        template<typename F, bool is_buffered, template<typename...> typename... Args>
        struct pusher<F, is_buffered, void, Args...> final
                : public pusher_base<is_buffered, void, Args...> {
            using base_t = pusher_base<is_buffered, void, Args...>;
            using channel_t = typename base_t::channel_t;

            template<typename G>
            explicit
            pusher(channel_t &channel, G &&func) : base_t(channel), func(std::forward<G>(func)) {}

            void operator()() final { std::invoke(this->func); }

            F func;
        };

    }  // namespace _detail

/** A "view" of a channel that only allows reading operations. */
    template<typename ChannelT>
    class channel_read_view {
    public:
        using value_type = typename ChannelT::value_type;

        explicit channel_read_view(ChannelT &channel) : _channel(channel) {}

        template<wait_type wtype, typename... Args>
        [[nodiscard]] auto pop_wt(Args &&... args) {
            return this->_channel.get().template pop_wt<wtype>(std::forward<Args>(args)...);
        }

        template<wait_type wtype, typename F, typename... Args>
        [[nodiscard]] auto pop_func_wt(F &&func, Args &&... args) {
            return this->_channel.get().template pop_func_wt<wtype>(std::forward<F>(func),
                                                                    std::forward<Args>(args)...);
        }

        [[nodiscard]] auto pop() { return this->_channel.get().pop(); }

        [[nodiscard]] auto try_pop() { return this->_channel.get().try_pop(); }

        template<typename Rep, typename Period>
        [[nodiscard]] auto try_pop_for(const std::chrono::duration<Rep, Period> &rel_time) {
            return this->_channel.get().try_pop_for(rel_time);
        }

        template<typename Clock, typename Duration>
        [[nodiscard]] auto
        try_pop_until(const std::chrono::time_point<Clock, Duration> &timeout_time) {
            return this->_channel.get().try_pop_until(timeout_time);
        }

        template<typename F>
        [[nodiscard]] auto pop_func(F &&func) {
            return this->_channel.get().pop_func(std::forward<F>(func));
        }

        template<typename F>
        [[nodiscard]] auto try_pop_func(F &&func) {
            return this->_channel.get().try_pop_func(std::forward<F>(func));
        }

        template<typename F, typename Rep, typename Period>
        [[nodiscard]] auto
        try_pop_func_for(F &&func, const std::chrono::duration<Rep, Period> &rel_time) {
            return this->_channel.get().try_pop_func_for(std::forward<F>(func), rel_time);
        }

        template<typename F, typename Clock, typename Duration>
        [[nodiscard]] auto
        try_pop_func_until(F &&func, const std::chrono::time_point<Clock, Duration> &timeout_time) {
            return this->_channel.get().try_pop_func_until(std::forward<F>(func), timeout_time);
        }

        [[nodiscard]] channel_pop_iterator<ChannelT>
        begin() { return this->_channel.get().begin(); }

        [[nodiscard]] channel_pop_iterator<ChannelT> end() { return this->_channel.get().end(); }

        [[nodiscard]] channel_read_view<ChannelT> read_view() {
            return channel_read_view(this->_channel);
        }

    protected:
        std::reference_wrapper<ChannelT> _channel;
    };

/** A "view" of a channel that only allows writing operations. */
    template<typename ChannelT>
    class channel_write_view {
    public:
        using value_type = typename ChannelT::value_type;

        explicit channel_write_view(ChannelT &channel) : _channel(channel) {}

        template<wait_type wtype,
                typename U,
                typename T_ = value_type,
                typename std::enable_if_t<!std::is_void_v<T_>, int> = 0,
                typename... Args>
        [[nodiscard]] auto push_wt(U &&data, Args &&... args) {
            return this->_channel.get().template push_wt<wtype>(std::forward<U>(data),
                                                                std::forward<Args>(args)...);
        }

        template<typename U, typename T_ = value_type, typename std::enable_if_t<!std::is_void_v<T_>, int> = 0>
        [[nodiscard]] auto push(U &&data) {
            return this->_channel.get().push(std::forward<U>(data));
        }

        template<typename U, typename T_ = value_type, typename std::enable_if_t<!std::is_void_v<T_>, int> = 0>
        [[nodiscard]] auto try_push(U &&data) {
            return this->_channel.get().try_push(std::forward<U>(data));
        }

        template<typename U,
                typename Rep,
                typename Period,
                typename T_ = value_type,
                typename std::enable_if_t<!std::is_void_v<T_>, int> = 0>
        [[nodiscard]] auto
        try_push_for(U &&data, const std::chrono::duration<Rep, Period> &rel_time) {
            return this->_channel.get().try_push_for(std::forward<U>(data), rel_time);
        }

        template<typename U,
                typename Clock,
                typename Duration,
                typename T_ = value_type,
                typename std::enable_if_t<!std::is_void_v<T_>, int> = 0>
        [[nodiscard]] auto
        try_push_until(U &&data, const std::chrono::time_point<Clock, Duration> &timeout_time) {
            return this->_channel.get().try_push_until(std::forward<U>(data), timeout_time);
        }

        template<wait_type wtype,
                typename T_ = value_type,
                typename std::enable_if_t<std::is_void_v<T_>, int> = 0,
                typename... Args>
        [[nodiscard]] auto push_wt(Args &&... args) {
            return this->_channel.get().template push_wt<wtype>(std::forward<Args>(args)...);
        }

        template<typename T_ = value_type, typename std::enable_if_t<std::is_void_v<T_>, int> = 0>
        [[nodiscard]] auto push() {
            return this->_channel.get().push();
        }

        template<typename T_ = value_type, typename std::enable_if_t<std::is_void_v<T_>, int> = 0>
        [[nodiscard]] auto try_push() {
            return this->_channel.get().try_push();
        }

        template<typename Rep,
                typename Period,
                typename T_ = value_type,
                typename std::enable_if_t<std::is_void_v<T_>, int> = 0>
        [[nodiscard]] auto try_push_for(const std::chrono::duration<Rep, Period> &rel_time) {
            return this->_channel.get().try_push_for(rel_time);
        }

        template<typename Clock,
                typename Duration,
                typename T_ = value_type,
                typename std::enable_if_t<std::is_void_v<T_>, int> = 0>
        [[nodiscard]] auto
        try_push_until(const std::chrono::time_point<Clock, Duration> &timeout_time) {
            return this->_channel.get().try_push_until(timeout_time);
        }

        template<wait_type wtype, typename F, typename... Args>
        [[nodiscard]] auto push_func_wt(F &&func, Args &&... args) {
            return this->_channel.get().template push_func_wt<wtype>(std::forward<F>(func),
                                                                     std::forward<Args>(args)...);
        }

        template<typename F>
        [[nodiscard]] auto push_func(F &&func) {
            return this->_channel.get().push_func(std::forward<F>(func));
        }

        template<typename F>
        [[nodiscard]] auto try_push_func(F &&func) {
            return this->_channel.get().try_push_func(std::forward<F>(func));
        }

        template<typename F, typename Rep, typename Period>
        [[nodiscard]] auto
        try_push_func_for(F &&func, const std::chrono::duration<Rep, Period> &rel_time) {
            return this->_channel.get().try_push_func_for(std::forward<F>(func), rel_time);
        }

        template<typename F, typename Clock, typename Duration>
        [[nodiscard]] auto try_push_func_until(F &&func,
                                               const std::chrono::time_point<Clock, Duration> &timeout_time) {
            return this->_channel.get().try_push_func_until(std::forward<F>(func), timeout_time);
        }

        [[nodiscard]] channel_push_iterator<ChannelT>
        push_iterator() { return this->_channel.get().push_iterator(); }

        [[nodiscard]] channel_write_view write_view() { return channel_write_view(this->_channel); }

        void close() { this->_channel.get().close(); }

        size_t clear() { return this->_channel.get().clear(); }

    private:
        std::reference_wrapper<ChannelT> _channel;
    };

/** An iterator to "pop" elements from a channel repeatedly. */
    template<typename ChannelT>
    class channel_pop_iterator {
    public:
        using value_type = typename ChannelT::value_type;
        using reference = std::add_lvalue_reference_t<value_type>;
        using pointer = value_type *;
        using difference_type = std::ptrdiff_t;
        using iterator_category = std::input_iterator_tag;

        channel_pop_iterator() : _channel(std::nullopt) {}

        explicit channel_pop_iterator(ChannelT &channel) : _channel(channel) {}

        auto operator++(int) &{
            auto proxy = _post_incr_proxy(std::move(**this));
            ++(*this);
            return proxy;
        }

        channel_pop_iterator &operator++() &{
            if (!this->_last_value) {
                this->_advance(false);
            } else {
                this->_last_value.reset();
            }
            return *this;
        }

        [[nodiscard]] reference operator*() const {
            if (!this->_last_value) {
                this->_advance(true);
            }
            return this->_last_value.value();
        }

        [[nodiscard]] pointer operator->() const { return &**this; }

        [[nodiscard]] bool operator==(const channel_pop_iterator &other) const {
            if (this->_last_value) {
                return false;
            }
            if (!this->_channel || this->_channel->get().is_read_closed()) {
                return !other._channel;
            }
            this->_advance(true);
            return other._channel.has_value() == this->_channel.has_value();
        }

        [[nodiscard]] bool operator!=(const channel_pop_iterator &other) const {
            return !(*this == other);
        }

    private:
        mutable std::optional<std::reference_wrapper<ChannelT>> _channel;
        mutable std::conditional_t<std::is_void_v<value_type>, bool, std::optional<value_type>> _last_value{};

        void _advance(bool store) const {
            assert(this->_channel);
            auto pop = this->_channel->get().pop();
            if (!pop) {
                this->_channel.reset();
                this->_last_value.reset();
            } else if (store) {
                this->_last_value = std::move(pop.value());
            }
        }

        struct _post_incr_proxy {
            template<typename U>
            _post_incr_proxy(U &&u) : value(std::forward<U>(u)) {}

            value_type operator*() { return std::move(value); }

            value_type value;
        };
    };

/** An iterator to "push" elements to a channel repeatedly. */
    template<typename ChannelT>
    class channel_push_iterator {
        struct _pusher;

    public:
        using value_type = _pusher;
        using reference = value_type &;
        using pointer = void;
        using difference_type = std::ptrdiff_t;
        using iterator_category = std::output_iterator_tag;

        channel_push_iterator() : _channel(std::nullopt) {}

        explicit channel_push_iterator(ChannelT &channel) : _channel(channel) {}

        channel_push_iterator operator++(int) &{ return *this; }

        channel_push_iterator &operator++() &{ return *this; }

        [[nodiscard]] _pusher operator*() const { return _pusher(this->_channel.value()); }

    private:
        std::optional<std::reference_wrapper<ChannelT>> _channel;

        struct _pusher {
            explicit _pusher(ChannelT &channel) : _channel(&channel) {}

            template<typename U>
#if my__cpp_concepts >= 201907L
            requires std::convertible_to<U, typename ChannelT::value_type>
#endif
            void operator=(U &&data) const { (void) this->_channel->push(std::forward<U>(data)); }

            ChannelT *_channel;
        };
    };

    namespace _detail {

        struct voidval_t {
        };

        template<bool _is_pop, typename VarT, bool is_buffered, typename T, template<typename...> typename... Args>
        struct value_select_token {
            static constexpr bool is_op_base = false;
            static constexpr bool is_pop = _is_pop;
            static constexpr bool is_void = false;
            using var_t = VarT;
            channel<is_buffered, T, Args...> &chan;
            VarT outvar;
        };

        template<bool _is_pop, bool is_buffered, template<typename...> typename... Args>
        struct value_select_token<_is_pop, void, is_buffered, void, Args...> {
            static constexpr bool is_op_base = false;
            static constexpr bool is_pop = _is_pop;
            static constexpr bool is_void = true;
            channel<is_buffered, void, Args...> &chan;
        };

        template<wait_type wtype, typename... Ops, typename... Args>
        channel_op_status opselect(std::tuple<Ops...> &&ops, Args &&... args) {
            auto manager = std::make_from_tuple<_detail::select_manager<std::remove_reference_t<Ops>...>>(
                    ops);
            return manager.template select<wtype>(std::forward<Args>(args)...);
        }

        template<wait_type wtype, typename... Pairs, typename... Args>
        channel_op_status valueselect(std::tuple<Pairs...> &&pairs, Args &&... args) {
            auto selected_index = size_t(0);
            auto pair_to_op = [&selected_index](size_t index, auto &pair) {
                using vst_t = typename std::remove_reference_t<decltype(pair.first)>;
                auto &vst = pair.first;
                auto &postfunc = pair.second;
                if constexpr (vst_t::is_pop) {
                    if constexpr (!vst_t::is_void) {
                        return vst.chan >> [var = &vst.outvar, &selected_index, index](auto &&x) {
                            *var = std::forward<decltype(x)>(x);
                            selected_index = index;
                        };
                    } else {
                        return vst.chan >> [&selected_index, index] { selected_index = index; };
                    }
                } else {
                    if constexpr (!vst_t::is_void) {
                        if constexpr (std::is_rvalue_reference_v<typename std::remove_reference_t<decltype(vst)>::var_t>) {
                            return vst.chan << [var = std::ref(
                                    vst.outvar), &selected_index, index]() -> auto && {
                                selected_index = index;
                                return std::move(var.get());
                            };
                        } else {
                            return vst.chan << [var = std::ref(
                                    vst.outvar), &selected_index, index]() -> auto & {
                                selected_index = index;
                                return var.get();
                            };
                        }
                    } else {
                        return vst.chan << [&selected_index, index] { selected_index = index; };
                    }
                }
            };
            const auto result = opselect<wtype>(transform_tuple(pairs, pair_to_op),
                                                std::forward<Args>(args)...);
            if (result == channel_op_status::success) {
                visit_at(pairs, selected_index, [](auto &&f) { f.second(); });
            }
            return result;
        }

        template<typename Rhs, bool is_buffered, typename T, template<typename...> typename... Args>
        struct channel_rshift_op {
            auto operator()(channel<is_buffered, T, Args...> &channel, Rhs &&rhs) const {
                if constexpr (std::is_invocable_v<Rhs &&, T &&>) {
                    return _detail::make_popper(channel, std::forward<Rhs>(rhs));
                } else {
                    COPPER_STATIC_ASSERT((std::is_same_v<Rhs &&, T &>));
                    return _detail::value_select_token<true, T &, is_buffered, T, Args...>{channel,
                                                                                           rhs};
                }
            }
        };

        template<typename Rhs, bool is_buffered, template<typename...> typename... Args>
        struct channel_rshift_op<Rhs, is_buffered, void, Args...> {
            auto operator()(channel<is_buffered, void, Args...> &channel, Rhs &&rhs) const {
                if constexpr (std::is_invocable_v<Rhs &&>) {
                    return _detail::make_popper(channel, std::forward<Rhs>(rhs));
                } else {
                    COPPER_STATIC_ASSERT(
                            (std::is_same_v<std::remove_cv_t<std::remove_reference_t<Rhs>>, _detail::voidval_t>));
                    return _detail::value_select_token<true, void, is_buffered, void, Args...>{
                            channel};
                }
            }
        };

        template<typename Rhs, bool is_buffered, typename T, template<typename...> typename... Args>
        struct channel_lshift_op {
            auto operator()(channel<is_buffered, T, Args...> &channel, Rhs &&rhs) const {
                if constexpr (std::is_invocable_r_v<T, Rhs &&>) {
                    return _detail::make_pusher(channel, std::forward<Rhs>(rhs));
                } else {
                    COPPER_STATIC_ASSERT(
                            (std::is_same_v<std::remove_cv_t<std::remove_reference_t<Rhs>>, T>));
                    return _detail::value_select_token<false, Rhs &&, is_buffered, T, Args...>{
                            channel, std::forward<Rhs>(rhs)};
                }
            }
        };

        template<typename Rhs, bool is_buffered, template<typename...> typename... Args>
        struct channel_lshift_op<Rhs, is_buffered, void, Args...> {
            auto operator()(channel<is_buffered, void, Args...> &channel, Rhs &&rhs) const {
                if constexpr (std::is_invocable_v<Rhs &&>) {
                    return _detail::make_pusher(channel, std::forward<Rhs>(rhs));
                } else {
                    COPPER_STATIC_ASSERT(
                            (std::is_same_v<std::remove_cv_t<std::remove_reference_t<Rhs>>, _detail::voidval_t>));
                    return _detail::value_select_token<false, void, is_buffered, void, Args...>{
                            channel};
                }
            }
        };

    }  // namespace _detail

    constexpr _detail::voidval_t voidval;
    constexpr _detail::voidval_t _;

    template<typename Rhs, bool is_buffered, typename T, template<typename...> typename... Args>
    [[nodiscard]] auto operator>>(channel<is_buffered, T, Args...> &channel, Rhs &&rhs) {
        return _detail::channel_rshift_op<Rhs, is_buffered, T, Args...>()(channel,
                                                                          std::forward<Rhs>(rhs));
    }

    template<typename Rhs, bool is_buffered, typename T, template<typename...> typename... Args>
    [[nodiscard]] auto operator<<(channel<is_buffered, T, Args...> &channel, Rhs &&rhs) {
        return _detail::channel_lshift_op<Rhs, is_buffered, T, Args...>()(channel,
                                                                          std::forward<Rhs>(rhs));
    }

/** User-level "select" function. */
    template<typename... Ops>
    [[nodiscard]] std::enable_if_t<(Ops::is_op_base && ...), channel_op_status>
    select(Ops &&... ops) {
        return _detail::opselect<wait_type::forever>(std::forward_as_tuple(ops...));
    }

/** User-level "select" function. */
    template<typename... Ops>
    [[nodiscard]] std::enable_if_t<(Ops::is_op_base && ...), channel_op_status>
    try_select(Ops &&... ops) {
        return _detail::opselect<wait_type::none>(std::forward_as_tuple(ops...));
    }

/** User-level "select" function. */
    template<typename Rep, typename Period, typename... Ops>
    [[nodiscard]] std::enable_if_t<(Ops::is_op_base && ...), channel_op_status> try_select_for(
            const std::chrono::duration<Rep, Period> &rel_time,
            Ops &&... ops) {
        return _detail::opselect<wait_type::for_>(std::forward_as_tuple(ops...), rel_time);
    }

/** User-level "select" function. */
    template<typename Clock, typename Duration, typename... Ops>
    [[nodiscard]] std::enable_if_t<(Ops::is_op_base && ...), channel_op_status> try_select_until(
            const std::chrono::time_point<Clock, Duration> &timeout_time,
            Ops &&... ops) {
        return _detail::opselect<wait_type::until>(std::forward_as_tuple(ops...), timeout_time);
    }

/** User-level "select" function. */
    template<typename... Args>
    [[nodiscard]] channel_op_status vselect(Args &&... args) {
        return _detail::template valueselect<wait_type::forever>(
                _detail::pairify_template<Args &&...>()(std::forward<Args>(args)...));
    }

/** User-level "select" function. */
    template<typename... Args>
    [[nodiscard]] channel_op_status try_vselect(Args &&... args) {
        return _detail::template valueselect<wait_type::none>(
                _detail::pairify_template<Args &&...>()(std::forward<Args>(args)...));
    }

/** User-level "select" function. */
    template<typename Rep, typename Period, typename... Args>
    [[nodiscard]] channel_op_status
    try_vselect_for(const std::chrono::duration<Rep, Period> &rel_time, Args &&... args) {
        return _detail::template valueselect<wait_type::for_>(
                _detail::pairify_template<Args &&...>()(std::forward<Args>(args)...),
                rel_time);
    }

/** User-level "select" function. */
    template<typename Clock, typename Duration, typename... Args>
    [[nodiscard]] channel_op_status
    try_vselect_until(const std::chrono::time_point<Clock, Duration> &timeout_time,
                      Args &&... args) {
        return _detail::template valueselect<wait_type::until>(
                _detail::pairify_template<Args &&...>()(std::forward<Args>(args)...),
                timeout_time);
    }

    namespace _detail {

/** select_manager takes care of all operations with in a call to a function of the "select" family.
 * This is a special case for a single op. */
        template<typename Op>
        struct select_manager<Op> {
            explicit select_manager(Op &op) : _op(&op) {}

            template<wait_type wtype, typename... Args>
            [[nodiscard]] channel_op_status select(Args &&... args) {
                return this->_op->template select<wtype>(std::forward<Args>(args)...);
            }

        private:
            Op *const _op;
        };

/** select_manager takes care of all operations with in a call to a function of the "select" family. */
        template<typename... Ops>
        struct select_manager {
            explicit select_manager(Ops &... ops) : _ops(ops...) {}

            select_manager(const select_manager &) = delete;

            select_manager(select_manager &&) = delete;

            select_manager &operator=(const select_manager &) = delete;

            select_manager &operator=(select_manager &&) = delete;

            ~select_manager() = default;

            /** Performs a general-purpose "select" operation with the given waiting strategy on the included operations. */
            template<wait_type wtype, typename... Args>
            channel_op_status select(Args &&... args) {
                const auto first_pass_status = this->_perform_select_pass();
                if (first_pass_status == channel_op_status::success) {
                    return channel_op_status::success;
                }
                if constexpr (wtype == wait_type::none) {
                    return first_pass_status;
                } else {
                    if (this->_register_signals()) {
                        return channel_op_status::success;
                    }
                    return this->_ops.template wait<wtype>(std::forward<Args>(args)...);
                }
            }

        private:
            waiting_op_group<Ops...> _ops;

            /** The first step that is done in a "select" call.
             * With as little overhead as possible, all ops are tried to be executed in a random order (to be fair over multiple
             * calls).
             * If any op could be selected, returns `success`. If all channels are closed, returns `closed`. Otherwise, returns
             * `unavailable`. */
            [[nodiscard]] channel_op_status _perform_select_pass() {
                // Find a random order of all passed ops.
                auto random_order_indices = std::array<size_t, sizeof...(Ops)>();
                fill_with_random_indices(random_order_indices);

                // Iterate over all ops in the random order.
                // Each op is attempted to be "selected".
                auto all_closed = true;
                for (const auto i: random_order_indices) {
                    auto status = channel_op_status{};
                    const auto f = [&status](auto *op) { status = op->try_select(); };
                    visit_at(this->_ops.ops, i, f);
                    if (status == channel_op_status::success) {
                        return channel_op_status::success;
                    } else if (status == channel_op_status::unavailable) {
                        all_closed = false;
                    }
                }
                return all_closed ? channel_op_status::closed : channel_op_status::unavailable;
            }

            /** If `_perform_select_pass` was not successful, `_register_signals` is called.
             * Try to run each op again while permanently locking them this time if they could not be executed.
             * Once the channels of all ops are locked, the ops can be registered in their waiting queue. */
            [[nodiscard]] bool _register_signals() {
                // Each op is attempted to be "selected". If it is, the whole process stops and returns `true`.
                // Otherwise, they are successively locked.
                auto already_locked = std::vector<std::pair<channel_mutex_t *, size_t>>();
                already_locked.reserve(sizeof...(Ops));
                auto locks = std::array<std::unique_lock<channel_mutex_t>, sizeof...(Ops)>();
                auto success = false;
                const auto f = [&success, &locks, &already_locked](size_t i, auto *op) {
                    if (success) {
                        return;
                    }
                    const auto already_locked_pair =
                            std::find_if(already_locked.begin(), already_locked.end(),
                                         [op](const auto &pair) {
                                             return pair.first == &op->channel_mutex();
                                         });
                    if (already_locked_pair == already_locked.end()) {
                        locks[i] = std::unique_lock<channel_mutex_t>(op->channel_mutex());
                        already_locked.emplace_back(&op->channel_mutex(), i);
                    }
                    success = op->try_select_locked() == channel_op_status::success;
                };
                for_each_in_tuple(this->_ops.ops, f);
                if (success) {
                    return true;
                }

                // No select operation could be performed immediately, so this select manager is enqueued to the channels.
                this->_ops.activate();
                return false;
            }
        };

    }  // namespace _detail

    namespace _detail {

        template<size_t I>
        struct visit_impl {
            template<typename T, typename F>
            static void visit(T &tup, size_t idx, F &&fun) {
                if (idx == I - 1) {
                    fun(std::get<I - 1>(tup));
                } else {
                    assert(I > 0);
                    visit_impl<I - 1>::visit(tup, idx, std::forward<F>(fun));
                }
            }
        };

        template<>
        struct visit_impl<0> {
            template<typename T, typename F>
            static void visit(T &tup, size_t idx, F &&fun) {}
        };

/** Calls a function with the ith element of a tuple. Basically std::get at runtime. */
        template<typename F, typename... Ts>
        void visit_at(const std::tuple<Ts...> &tup, size_t idx, F &&fun) {
            visit_impl<sizeof...(Ts)>::visit(tup, idx, std::forward<F>(fun));
        }

/** Calls a function with the ith element of a tuple. Basically std::get at runtime. */
        template<typename F, typename... Ts>
        void visit_at(std::tuple<Ts...> &tup, size_t idx, F &&fun) {
            visit_impl<sizeof...(Ts)>::visit(tup, idx, std::forward<F>(fun));
        }

        template<typename T, typename F, size_t... Is>
        void for_each_in_tuple_impl(T &&t, F &&f, std::integer_sequence<size_t, Is...>) {
            auto l = {(f(Is, std::get<Is>(t)), 0)...};
            (void) l;
        }

/** Calls a functor object for each element in a tuple. */
        template<typename... Ts, typename F>
        void for_each_in_tuple(const std::tuple<Ts...> &t, F &&f) {
            for_each_in_tuple_impl(t, std::forward<F>(f),
                                   std::make_integer_sequence<size_t, sizeof...(Ts)>());
        }

/** convert a tuple (A, B, C, D, ...) to a tuple of pairs ((A, B), (C, D), ...) */
        template<>
        struct pairify_template<> {
            std::tuple<> operator()() { return {}; }
        };

        template<typename A, typename B, typename... Args>
        struct pairify_template<A, B, Args...> {
            auto operator()(A a, B b, Args &&... args) {
                return std::tuple_cat(
                        std::make_tuple(std::pair<A, B>(std::forward<A>(a), std::forward<B>(b))),
                        pairify_template<Args...>()(std::forward<Args>(args)...));
            }
        };

/** apply a function to each element of a tuple and return the result */
        template<typename T, typename F, size_t... Is>
        auto transform_tuple_impl(T &&t, F &&f, std::integer_sequence<size_t, Is...>) {
            return std::make_tuple((f(Is, std::get<Is>(t)))...);
        }

        template<typename... Ts, typename F>
        auto transform_tuple(const std::tuple<Ts...> &t, F &&f) {
            return transform_tuple_impl(t, std::forward<F>(f),
                                        std::make_integer_sequence<size_t, sizeof...(Ts)>());
        }

/** Based on xoshiro128++ by by David Blackman and Sebastiano Vigna (vigna@acm.org)
 * Fast 32-bit PRN generator that can be constructed multiple times, yielding different seeds every time. */
        class thread_local_randomizer {
        public:
            using result_type = unsigned int;//std::uint32_t;
            using seed_type = std::array<result_type, 4>;

            /** Initializes a new `thread_local_randomizer` instance with a new seed. */
            static thread_local_randomizer create() {
                static auto global_seed = _initialize_global_seed();
                static auto mutex = std::mutex();
                const auto lock = std::lock_guard(mutex);
                auto result = thread_local_randomizer(global_seed);
                _jump_seed(global_seed);
                return result;
            }

            /** Generates a new random number. */
            result_type operator()(result_type end) { return _next_seed(this->_seed) % end; }

        private:
            explicit thread_local_randomizer(const seed_type &seed) : _seed(seed) {}

            static seed_type _initialize_global_seed() {
                auto rd = std::random_device();
                return {rd(), rd(), rd(), rd()};
            }

            static inline result_type _xxrotl(result_type x, int k) {
                return (x << k) | (x >> (32 - k));
            }

            static void _jump_seed(seed_type &seed) {
                static const result_type JUMP[] = {0x8764000b, 0xf542d2d3, 0x6fa035c3, 0x77f2db5b};

                result_type s0 = 0;
                result_type s1 = 0;
                result_type s2 = 0;
                result_type s3 = 0;
                for (const auto jump: JUMP)
                    for (int b = 0; b < 32; b++) {
                        if (jump & UINT32_C(1) << b) {
                            s0 ^= seed[0];
                            s1 ^= seed[1];
                            s2 ^= seed[2];
                            s3 ^= seed[3];
                        }
                        _next_seed(seed);
                    }

                seed[0] = s0;
                seed[1] = s1;
                seed[2] = s2;
                seed[3] = s3;
            }

            static result_type _next_seed(seed_type &seed) {
                const auto result = _xxrotl(seed[0] + seed[3], 7) + seed[0];

                const result_type t = seed[1] << 9;

                seed[2] ^= seed[0];
                seed[3] ^= seed[1];
                seed[1] ^= seed[2];
                seed[0] ^= seed[3];

                seed[2] ^= t;

                seed[3] = _xxrotl(seed[3], 11);

                return result;
            }

            seed_type _seed;
        };

/** Helper implementation for `fill_with_random_indices`. */
        template<size_t N>
        struct fill_with_random_indices_impl {
            void operator()(std::array<size_t, N> &indices) {
                thread_local auto engine = thread_local_randomizer::create();
                struct urbg {
                    using result_type = decltype(engine(N));

                    static constexpr result_type (min)() { return 0; }

                    static constexpr result_type (max)() { return N - 1; }

                    result_type operator()() { return engine(N); }
                };
                std::iota(indices.begin(), indices.end(), 0);
                std::shuffle(indices.begin(), indices.end(), urbg{});
            }
        };

        template<>
        struct fill_with_random_indices_impl<1> {
            void operator()(std::array<size_t, 1> &indices) { indices[0] = 0; }
        };

        template<>
        struct fill_with_random_indices_impl<2> {
            void operator()(std::array<size_t, 2> &indices) {
                thread_local auto engine = thread_local_randomizer::create();
                switch (engine(2)) {
                    case 0:
                        indices = {0, 1};
                        break;
                    case 1:
                        indices = {1, 0};
                        break;
                }
            }
        };

        template<>
        struct fill_with_random_indices_impl<3> {
            void operator()(std::array<size_t, 3> &indices) {
                thread_local auto engine = thread_local_randomizer::create();
                switch (engine(6)) {
                    case 0:
                        indices = {0, 1, 2};
                        break;
                    case 1:
                        indices = {0, 2, 1};
                        break;
                    case 2:
                        indices = {1, 0, 2};
                        break;
                    case 3:
                        indices = {1, 2, 0};
                        break;
                    case 4:
                        indices = {2, 0, 1};
                        break;
                    case 5:
                        indices = {2, 1, 0};
                        break;
                }
            }
        };

        template<>
        struct fill_with_random_indices_impl<4> {
            void operator()(std::array<size_t, 4> &indices) {
                thread_local auto engine = thread_local_randomizer::create();
                switch (engine(24)) {
                    case 0:
                        indices = {0, 1, 2, 3};
                        break;
                    case 1:
                        indices = {0, 1, 3, 2};
                        break;
                    case 2:
                        indices = {0, 2, 1, 3};
                        break;
                    case 3:
                        indices = {0, 2, 3, 1};
                        break;
                    case 4:
                        indices = {0, 3, 1, 2};
                        break;
                    case 5:
                        indices = {0, 3, 2, 1};
                        break;
                    case 6:
                        indices = {1, 0, 2, 3};
                        break;
                    case 7:
                        indices = {1, 0, 3, 2};
                        break;
                    case 8:
                        indices = {1, 2, 0, 3};
                        break;
                    case 9:
                        indices = {1, 2, 3, 0};
                        break;
                    case 10:
                        indices = {1, 3, 0, 2};
                        break;
                    case 11:
                        indices = {1, 3, 2, 0};
                        break;
                    case 12:
                        indices = {2, 0, 1, 3};
                        break;
                    case 13:
                        indices = {2, 0, 3, 1};
                        break;
                    case 14:
                        indices = {2, 1, 0, 3};
                        break;
                    case 15:
                        indices = {2, 1, 3, 0};
                        break;
                    case 16:
                        indices = {2, 3, 0, 1};
                        break;
                    case 17:
                        indices = {2, 3, 1, 0};
                        break;
                    case 18:
                        indices = {3, 0, 1, 2};
                        break;
                    case 19:
                        indices = {3, 0, 2, 1};
                        break;
                    case 20:
                        indices = {3, 1, 0, 2};
                        break;
                    case 21:
                        indices = {3, 1, 2, 0};
                        break;
                    case 22:
                        indices = {3, 2, 0, 1};
                        break;
                    case 23:
                        indices = {3, 2, 1, 0};
                        break;
                }
            }
        };

        template<>
        struct fill_with_random_indices_impl<5> {
            void operator()(std::array<size_t, 5> &indices) {
                thread_local auto engine = thread_local_randomizer::create();
                switch (engine(120)) {
                    case 0:
                        indices = {0, 1, 2, 3, 4};
                        break;
                    case 1:
                        indices = {0, 1, 2, 4, 3};
                        break;
                    case 2:
                        indices = {0, 1, 3, 2, 4};
                        break;
                    case 3:
                        indices = {0, 1, 3, 4, 2};
                        break;
                    case 4:
                        indices = {0, 1, 4, 2, 3};
                        break;
                    case 5:
                        indices = {0, 1, 4, 3, 2};
                        break;
                    case 6:
                        indices = {0, 2, 1, 3, 4};
                        break;
                    case 7:
                        indices = {0, 2, 1, 4, 3};
                        break;
                    case 8:
                        indices = {0, 2, 3, 1, 4};
                        break;
                    case 9:
                        indices = {0, 2, 3, 4, 1};
                        break;
                    case 10:
                        indices = {0, 2, 4, 1, 3};
                        break;
                    case 11:
                        indices = {0, 2, 4, 3, 1};
                        break;
                    case 12:
                        indices = {0, 3, 1, 2, 4};
                        break;
                    case 13:
                        indices = {0, 3, 1, 4, 2};
                        break;
                    case 14:
                        indices = {0, 3, 2, 1, 4};
                        break;
                    case 15:
                        indices = {0, 3, 2, 4, 1};
                        break;
                    case 16:
                        indices = {0, 3, 4, 1, 2};
                        break;
                    case 17:
                        indices = {0, 3, 4, 2, 1};
                        break;
                    case 18:
                        indices = {0, 4, 1, 2, 3};
                        break;
                    case 19:
                        indices = {0, 4, 1, 3, 2};
                        break;
                    case 20:
                        indices = {0, 4, 2, 1, 3};
                        break;
                    case 21:
                        indices = {0, 4, 2, 3, 1};
                        break;
                    case 22:
                        indices = {0, 4, 3, 1, 2};
                        break;
                    case 23:
                        indices = {0, 4, 3, 2, 1};
                        break;
                    case 24:
                        indices = {1, 0, 2, 3, 4};
                        break;
                    case 25:
                        indices = {1, 0, 2, 4, 3};
                        break;
                    case 26:
                        indices = {1, 0, 3, 2, 4};
                        break;
                    case 27:
                        indices = {1, 0, 3, 4, 2};
                        break;
                    case 28:
                        indices = {1, 0, 4, 2, 3};
                        break;
                    case 29:
                        indices = {1, 0, 4, 3, 2};
                        break;
                    case 30:
                        indices = {1, 2, 0, 3, 4};
                        break;
                    case 31:
                        indices = {1, 2, 0, 4, 3};
                        break;
                    case 32:
                        indices = {1, 2, 3, 0, 4};
                        break;
                    case 33:
                        indices = {1, 2, 3, 4, 0};
                        break;
                    case 34:
                        indices = {1, 2, 4, 0, 3};
                        break;
                    case 35:
                        indices = {1, 2, 4, 3, 0};
                        break;
                    case 36:
                        indices = {1, 3, 0, 2, 4};
                        break;
                    case 37:
                        indices = {1, 3, 0, 4, 2};
                        break;
                    case 38:
                        indices = {1, 3, 2, 0, 4};
                        break;
                    case 39:
                        indices = {1, 3, 2, 4, 0};
                        break;
                    case 40:
                        indices = {1, 3, 4, 0, 2};
                        break;
                    case 41:
                        indices = {1, 3, 4, 2, 0};
                        break;
                    case 42:
                        indices = {1, 4, 0, 2, 3};
                        break;
                    case 43:
                        indices = {1, 4, 0, 3, 2};
                        break;
                    case 44:
                        indices = {1, 4, 2, 0, 3};
                        break;
                    case 45:
                        indices = {1, 4, 2, 3, 0};
                        break;
                    case 46:
                        indices = {1, 4, 3, 0, 2};
                        break;
                    case 47:
                        indices = {1, 4, 3, 2, 0};
                        break;
                    case 48:
                        indices = {2, 0, 1, 3, 4};
                        break;
                    case 49:
                        indices = {2, 0, 1, 4, 3};
                        break;
                    case 50:
                        indices = {2, 0, 3, 1, 4};
                        break;
                    case 51:
                        indices = {2, 0, 3, 4, 1};
                        break;
                    case 52:
                        indices = {2, 0, 4, 1, 3};
                        break;
                    case 53:
                        indices = {2, 0, 4, 3, 1};
                        break;
                    case 54:
                        indices = {2, 1, 0, 3, 4};
                        break;
                    case 55:
                        indices = {2, 1, 0, 4, 3};
                        break;
                    case 56:
                        indices = {2, 1, 3, 0, 4};
                        break;
                    case 57:
                        indices = {2, 1, 3, 4, 0};
                        break;
                    case 58:
                        indices = {2, 1, 4, 0, 3};
                        break;
                    case 59:
                        indices = {2, 1, 4, 3, 0};
                        break;
                    case 60:
                        indices = {2, 3, 0, 1, 4};
                        break;
                    case 61:
                        indices = {2, 3, 0, 4, 1};
                        break;
                    case 62:
                        indices = {2, 3, 1, 0, 4};
                        break;
                    case 63:
                        indices = {2, 3, 1, 4, 0};
                        break;
                    case 64:
                        indices = {2, 3, 4, 0, 1};
                        break;
                    case 65:
                        indices = {2, 3, 4, 1, 0};
                        break;
                    case 66:
                        indices = {2, 4, 0, 1, 3};
                        break;
                    case 67:
                        indices = {2, 4, 0, 3, 1};
                        break;
                    case 68:
                        indices = {2, 4, 1, 0, 3};
                        break;
                    case 69:
                        indices = {2, 4, 1, 3, 0};
                        break;
                    case 70:
                        indices = {2, 4, 3, 0, 1};
                        break;
                    case 71:
                        indices = {2, 4, 3, 1, 0};
                        break;
                    case 72:
                        indices = {3, 0, 1, 2, 4};
                        break;
                    case 73:
                        indices = {3, 0, 1, 4, 2};
                        break;
                    case 74:
                        indices = {3, 0, 2, 1, 4};
                        break;
                    case 75:
                        indices = {3, 0, 2, 4, 1};
                        break;
                    case 76:
                        indices = {3, 0, 4, 1, 2};
                        break;
                    case 77:
                        indices = {3, 0, 4, 2, 1};
                        break;
                    case 78:
                        indices = {3, 1, 0, 2, 4};
                        break;
                    case 79:
                        indices = {3, 1, 0, 4, 2};
                        break;
                    case 80:
                        indices = {3, 1, 2, 0, 4};
                        break;
                    case 81:
                        indices = {3, 1, 2, 4, 0};
                        break;
                    case 82:
                        indices = {3, 1, 4, 0, 2};
                        break;
                    case 83:
                        indices = {3, 1, 4, 2, 0};
                        break;
                    case 84:
                        indices = {3, 2, 0, 1, 4};
                        break;
                    case 85:
                        indices = {3, 2, 0, 4, 1};
                        break;
                    case 86:
                        indices = {3, 2, 1, 0, 4};
                        break;
                    case 87:
                        indices = {3, 2, 1, 4, 0};
                        break;
                    case 88:
                        indices = {3, 2, 4, 0, 1};
                        break;
                    case 89:
                        indices = {3, 2, 4, 1, 0};
                        break;
                    case 90:
                        indices = {3, 4, 0, 1, 2};
                        break;
                    case 91:
                        indices = {3, 4, 0, 2, 1};
                        break;
                    case 92:
                        indices = {3, 4, 1, 0, 2};
                        break;
                    case 93:
                        indices = {3, 4, 1, 2, 0};
                        break;
                    case 94:
                        indices = {3, 4, 2, 0, 1};
                        break;
                    case 95:
                        indices = {3, 4, 2, 1, 0};
                        break;
                    case 96:
                        indices = {4, 0, 1, 2, 3};
                        break;
                    case 97:
                        indices = {4, 0, 1, 3, 2};
                        break;
                    case 98:
                        indices = {4, 0, 2, 1, 3};
                        break;
                    case 99:
                        indices = {4, 0, 2, 3, 1};
                        break;
                    case 100:
                        indices = {4, 0, 3, 1, 2};
                        break;
                    case 101:
                        indices = {4, 0, 3, 2, 1};
                        break;
                    case 102:
                        indices = {4, 1, 0, 2, 3};
                        break;
                    case 103:
                        indices = {4, 1, 0, 3, 2};
                        break;
                    case 104:
                        indices = {4, 1, 2, 0, 3};
                        break;
                    case 105:
                        indices = {4, 1, 2, 3, 0};
                        break;
                    case 106:
                        indices = {4, 1, 3, 0, 2};
                        break;
                    case 107:
                        indices = {4, 1, 3, 2, 0};
                        break;
                    case 108:
                        indices = {4, 2, 0, 1, 3};
                        break;
                    case 109:
                        indices = {4, 2, 0, 3, 1};
                        break;
                    case 110:
                        indices = {4, 2, 1, 0, 3};
                        break;
                    case 111:
                        indices = {4, 2, 1, 3, 0};
                        break;
                    case 112:
                        indices = {4, 2, 3, 0, 1};
                        break;
                    case 113:
                        indices = {4, 2, 3, 1, 0};
                        break;
                    case 114:
                        indices = {4, 3, 0, 1, 2};
                        break;
                    case 115:
                        indices = {4, 3, 0, 2, 1};
                        break;
                    case 116:
                        indices = {4, 3, 1, 0, 2};
                        break;
                    case 117:
                        indices = {4, 3, 1, 2, 0};
                        break;
                    case 118:
                        indices = {4, 3, 2, 0, 1};
                        break;
                    case 119:
                        indices = {4, 3, 2, 1, 0};
                        break;
                }
            }
        };

/** Fills the given array with numbers from 0 to N-1 in a random order. */
        template<size_t N>
        static void fill_with_random_indices(std::array<size_t, N> &indices) {
            fill_with_random_indices_impl<N>()(indices);
        }

    }  // namespace _detail

}  // namespace copper

#endif  // INCLUDE_COPPER_H
