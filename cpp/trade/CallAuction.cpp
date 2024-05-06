/*************************************************************************
    > File Name: CallAuction.cpp
    > Author: Netcan
    > Blog: https://netcan.github.io/
    > Mail: netcan1996@gmail.com
    > Created Time: 2024-05-06 21:42
************************************************************************/
// ref: https://baike.baidu.com/item/%E9%9B%86%E5%90%88%E7%AB%9E%E4%BB%B7/460444

#include <cmath>
#include <cassert>
#include <iostream>
#include <cstdio>
#include <map>
#include <algorithm>
#include <random>

using volume_type = unsigned;
using order_id_t = int64_t;
enum Side {
    Buy,
    Sell
};

struct OrderInsert {
    order_id_t order_id;
    double price;
    volume_type volume;
    Side side;
    bool operator==(const OrderInsert& rhs) const {
        return order_id == rhs.order_id;
    }
};

#define logw(fmt, ...) printf("%s: " fmt "\n", __FUNCTION__, ##__VA_ARGS__)
#define logi(fmt, ...) printf("%s: " fmt "\n", __FUNCTION__, ##__VA_ARGS__)
#define logd(fmt, ...)

struct OrderBook {
    void on_order_insert(const OrderInsert& order_insert) {
        if (orders_.find(order_insert.order_id) != orders_.end()) {
            logw("insert order id failed");
            return;
        }
        orders_[order_insert.order_id] = order_insert;

        int price = static_cast<int>(order_insert.price * 100 + 0.5);
        logd("price: %d(%lf) volume: %d side: %s", price, order_insert.price,
             order_insert.volume, order_insert.side == Buy ? "Buy" : "Sell");

        auto [piviot, _] = volume_.insert({price, {}});
        if (order_insert.side == Buy) {
            if (piviot->second.buy_volume == 0) {
                auto it = piviot;
                for (; it != volume_.end(); ++it) {
                    if (it->second.buy_volume != 0) {
                        break;
                    }
                }
                if (it != volume_.end()) {
                    piviot->second.buy_volume += it->second.buy_volume;
                }
            }

            piviot->second.buy_volume += order_insert.volume;
            for (auto it = volume_.begin(); it != piviot; ++it) {
                if (it->second.buy_volume != 0) {
                    it->second.buy_volume += order_insert.volume;
                }
            }
        } else {
            if (piviot->second.sell_volume == 0) {
                auto it = piviot;
                for (; it != volume_.begin(); --it) {
                    if (it->second.sell_volume != 0) {
                        break;
                    }
                }
                piviot->second.sell_volume += it->second.sell_volume;
            }

            piviot->second.sell_volume += order_insert.volume;
            for (piviot++; piviot != volume_.end(); ++piviot) {
                if (piviot->second.sell_volume != 0) {
                    piviot->second.sell_volume += order_insert.volume;
                }
            }
        }
    }

    void on_order_cancel(int64_t order_id) {
        auto order_it = orders_.find(order_id);
        if (order_it == orders_.end()) {
            logw("insert order id failed");
            return;
        }
        auto price = static_cast<int>(order_it->second.price * 100 + 0.5);
        auto piviot = volume_.find(price);
        logd("price: %d(%lf) volume: %d side: %s", price,
             order_it->second.price, order_it->second.volume,
             order_it->second.side == Buy ? "Buy" : "Sell");

        if (order_it->second.side == Buy) {
            piviot->second.buy_volume -= order_it->second.volume;
            for (auto it = volume_.begin(); it != piviot; ++it) {
                if (it->second.buy_volume != 0) {
                    it->second.buy_volume -= order_it->second.volume;
                }
            }
        } else {
            for (auto it = piviot; it != volume_.end(); ++it) {
                if (it->second.sell_volume != 0) {
                    it->second.sell_volume -= order_it->second.volume;
                }
            }
        }

        orders_.erase(order_it);
    }

    std::pair<double, volume_type> get_opening_price_and_volume() {
        dump_volume();
        int best_price {};
        int best_volume {};

        for (const auto& [price, order]: volume_) {
            auto volume = order.max_volume();
            if (best_volume <= volume) {
                best_price = price;
                best_volume = volume;
                logd("find best_price: %d best_volume: %d", best_price, best_volume);
            }
        }
        return { best_price / 100.0, best_volume };
    }

    bool operator==(const OrderBook& rhs) const {
        return std::tie(orders_, volume_) == std::tie(rhs.orders_, rhs.volume_);
    };

private:
    void dump_volume() {
        logi("---------------------------------------");
        logi("buy_volume price sell_volume max_volume");
        for (auto it = volume_.crbegin(); it != volume_.crend(); it++) {
            const auto& [price, order] = *it;
            logi("%10d %5d %11d %10d", order.buy_volume, price, order.sell_volume, order.max_volume());
        }
    }

private:
    using price_t = int;
    struct order {
        volume_type buy_volume {};
        volume_type sell_volume {};
        volume_type max_volume() const {
            return std::min(buy_volume, sell_volume);
        }
        bool operator==(const order& rhs) const {
            return std::tie(buy_volume, sell_volume) ==
                   std::tie(rhs.buy_volume, rhs.sell_volume);
        }
    };
    std::map<order_id_t, OrderInsert> orders_;
    std::map<price_t, order> volume_;
};

OrderBook batch_insert(std::vector<OrderInsert>& orders) {
    OrderBook book;
    for (const auto& order: orders) {
        book.on_order_insert(order);
    }
    return book;
}

int main(int argc, char** argv) {
    std::vector<OrderInsert> orders {
        {__LINE__, 9.7, 300, Buy},
        {__LINE__, 10.3, 300, Sell},
        {__LINE__, 10.4, 200, Sell},
        {__LINE__, 10.0, 300, Buy},
        {__LINE__, 10.2, 150, Buy},
        {__LINE__, 10.0, 100, Sell},
        {__LINE__, 10.2, 500, Sell},
        {__LINE__, 10.1, 200, Sell},
        {__LINE__, 10.1, 200, Buy},
        {__LINE__, 9.8, 600, Buy},
        {__LINE__, 10.5, 100, Sell},
        {__LINE__, 10.3, 150, Buy},
        {__LINE__, 9.9, 500, Buy},
    };

    // initial
    auto book = batch_insert(orders);
    auto [best_price, best_volume] = book.get_opening_price_and_volume();
    logi("best_price: %f best_volume: %d", best_price, best_volume);

    // for test
    std::random_device rd;
    std::mt19937 g(rd());
    for (int k = 0; k < 1000; ++k) {
        std::shuffle(orders.begin(), orders.end(), g);
        auto test_book = batch_insert(orders);
        assert(book == test_book);
    }

    return 0;
}
