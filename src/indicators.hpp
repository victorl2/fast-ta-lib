#ifndef FAST_TA_INDICATORS
#define FAST_TA_INDICATORS

#include <vector>
#include <iostream>
#include <stdint.h>
#include <limits>
#include <cmath>

namespace fta
{

    enum PositionType
    {
        LONG,
        SHORT
    };
    enum OrderType
    {
        MARKET,
        LIMIT
    };
    
    enum TimeInForce
    {
        GTC,
        IMC,
        FOK,
        POST
    }; //GoodTillCancel, ImmediateOrCancel, FillOrKill, PostOnly

    class Order
    {
    public:
        PositionType direction; //long(buy) or short(sell)
        OrderType type;
        std::string symbol; //ex: BTCUSD
        double price;       //not applicable to market orders
        double quantity;    //position size, ex: 1300 units of USD
        double take_profit; //target price for placing a take profit order
        double stop_loss;   //targer price for placing a stop loss order
        TimeInForce in_force;

        Order(){

        }

        //Market order constructor
        Order(std::string pair, PositionType side, double size, double profit, double stop):symbol(pair) {
            type = MARKET;
            direction = side;
            quantity = size;
            take_profit = profit;
            stop_loss = stop;
            in_force = GTC;
        }

        //Limit order constructor
        Order(std::string pair, PositionType side, double size, double target_price, double profit, double stop): symbol(pair){
            type = LIMIT;
            direction = side;
            quantity = size;
            price = target_price;
            take_profit = profit;
            stop_loss = stop;
            in_force = POST;
        }
    };

    class PriceCandle
    {
    public:
        std::string time;
        double open_price;
        double close_price;
        double max_price;
        double min_price;
        double volume_traded;

        PriceCandle(std::string close_time, double open, double high, double low, double close, double volume)
        {
            time = close_time;
            open_price = open;
            max_price = high;
            min_price = low;
            close_price = close;
            volume_traded = volume;
        }
    };

    class TradePosition
    {
    public:
        PositionType type;    // denotes if the trade was a long or short position
        double entry_price;   // price when the position was opened
        double close_price;   //price when the position was closed
        double position_size; //total position size including leverage
        std::string time;     //time when the position was opened
        int leverage_used;    //leverage usage in the position
        double realized_pnl;  // amount of profit or loss for the resulting trade
        double total_paid_fee;

        TradePosition() {}

        TradePosition(std::string entry_time, int leverage, double open_price, double pos_size)
        {
            time = entry_time;
            leverage_used = leverage;
            entry_price = open_price;
            position_size = pos_size;
        }
    };

    std::ostream &operator<<(std::ostream &strm, const TradePosition &p)
    {
        return strm << "TradePosition(" << p.time << ", " << p.realized_pnl << ", " << p.entry_price << ", " << p.close_price
                    << ", " << p.position_size << ", " << p.leverage_used << "x )";
    }

    //Calculates the amount of capital in percentage that should be allocated weighting the probability with the project value to win
    inline double kelly_criterion(const double win_rate, const double total_value_won, const double total_value_lost)
    {
        if (total_value_lost <= 0)
            return 0.0;
        return win_rate - ((1 - win_rate) / (total_value_won / total_value_lost));
    }

    //Exponentially weighted moving average - alpha must be within [0,1]
    //equation shoudl be EWMA(i) = alpha * EWMA(i-1) + (1 - alpha) * current_price
    inline double ewma(const double current_price, const double last_ewma, const double alpha)
    {
        if (last_ewma == 0)
            return current_price;
        return alpha * last_ewma + (1 - alpha) * current_price;
    }

    //Exponentially weighted moving average - alpha must be within [0,1]
    inline double ewma(const std::vector<PriceCandle> &closing_prices, const uint32_t evaluation_size, const double alpha)
    {
        int last_value = closing_prices.size() - evaluation_size;
        double current_ewma = 0;
        for (int i = closing_prices.size() - 1; i >= last_value; i--)
            current_ewma = ewma(closing_prices[i].close_price, current_ewma, alpha);
        return current_ewma;
    }

    //Relative strength index using simple moving average
    inline double rsi_sma(const std::vector<PriceCandle> &closing_prices, uint32_t evaluation_size)
    {
        int last_value = closing_prices.size() - evaluation_size;
        double up_prices = 0;
        double down_prices = 0;
        double last_price = closing_prices[closing_prices.size() - 1].close_price;

        for (int i = closing_prices.size() - 2; i >= last_value; i--)
        {
            if (closing_prices[i].close_price > last_price)
                up_prices += closing_prices[i].close_price;
            else if (closing_prices[i].close_price < last_price)
                down_prices += closing_prices[i].close_price;
            last_price = closing_prices[i].close_price;
        }

        up_prices /= evaluation_size;
        down_prices /= evaluation_size;

        return 100 - 100 / (1 + up_prices / down_prices);
    }

    //Relative strength index using exponentially weighted moving average
    inline double rsi_ewma(const std::vector<PriceCandle> &closing_prices, const uint32_t evaluation_size, const double alpha)
    {
        int last_value = closing_prices.size() - evaluation_size;
        double last_price = closing_prices[closing_prices.size() - 1].close_price;
        double up_ewma = 0;
        double down_ewma = 0;

        for (int i = closing_prices.size() - 2; i >= last_value; i--)
        {
            if (closing_prices[i].close_price > last_price)
                up_ewma = ewma(closing_prices[i].close_price, up_ewma, alpha);
            else if (closing_prices[i].close_price < last_price)
                down_ewma = ewma(closing_prices[i].close_price, up_ewma, alpha);
            last_price = closing_prices[i].close_price;
        }
        return 100 - 100 / (1 + up_ewma / down_ewma);
    }

    //Simple moving average
    inline double sma(const std::vector<PriceCandle> &closing_prices, const uint32_t evaluation_size)
    {
        double sum = 0;
        int last_value = closing_prices.size() - evaluation_size;
        for (int i = closing_prices.size() - 1; i >= last_value; --i)
        {
            sum += closing_prices[i].close_price;
        }
        return sum / evaluation_size;
    }

    // raising falling rate
    inline double rr(double current_price, double last_price)
    {
        return (current_price - last_price) / last_price;
    }

    //Percent difference from the simple moving average in a given period
    inline double pd(double current_price, double sma_on_period)
    {

        return ((current_price - sma_on_period) / sma_on_period) * 100;
    }

    inline double pd(const std::vector<PriceCandle> &closing_prices, const uint32_t evaluation_size)
    {
        double sma_period = sma(closing_prices, evaluation_size);
        return pd(closing_prices[closing_prices.size() - 1].close_price, sma_period);
    }
} // namespace fta
#endif
