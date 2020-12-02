#ifndef FAST_TA_INDICATORS
#define FAST_TA_INDICATORS

#include <vector>
#include <iostream>
#include <stdint.h>
#include <limits>
#include <cmath>

namespace fta {
    //Exponentially weighted moving average - alpha must be within [0,1]
    //equation shoudl be EWMA(i) = alpha * EWMA(i-1) + (1 - alpha) * current_price
    inline double ewma(const double current_price, const double last_ewma, const double alpha){
        if(last_ewma == 0) return current_price;
        return alpha * last_ewma + (1 - alpha) * current_price;
    }

    //Exponentially weighted moving average - alpha must be within [0,1]
    inline double ewma(const std::vector<double>& closing_prices, const uint32_t evaluation_size, const double alpha){
        int last_value = closing_prices.size() - evaluation_size;
        double current_ewma = 0;
        for(int i = closing_prices.size() - 1; i >=  last_value; i--)
            current_ewma = ewma(closing_prices[i], current_ewma, alpha);
        return current_ewma;
    }

    //Relative strength index using simple moving average
    inline double rsi_sma(const std::vector<double>& closing_prices, uint32_t evaluation_size){
        int last_value = closing_prices.size() - evaluation_size;
        double up_prices = 0;
        double down_prices = 0;
        double last_price = closing_prices[closing_prices.size() - 1];
        
        for(int i = closing_prices.size() - 2; i >= last_value; i--){
            if(closing_prices[i] > last_price)
                up_prices+= closing_prices[i];
            else if(closing_prices[i] < last_price)
                down_prices += closing_prices[i];
            last_price = closing_prices[i];
        }

        up_prices /= evaluation_size;
        down_prices /= evaluation_size;

        return 100 - 100 / (1 + up_prices / down_prices);
    }

    //Relative strength index using exponentially weighted moving average
    inline double rsi_ewma(const std::vector<double>& closing_prices, const uint32_t evaluation_size, const double alpha){
        int last_value = closing_prices.size() - evaluation_size;
        double last_price = closing_prices[closing_prices.size() - 1];
        double up_ewma = 0;
        double down_ewma = 0;
     
        for(int i = closing_prices.size() - 2; i >= last_value; i--){
            if(closing_prices[i] > last_price)
                up_ewma= ewma(closing_prices[i], up_ewma, alpha);
            else if(closing_prices[i] < last_price)
                down_ewma = ewma(closing_prices[i], up_ewma, alpha);
            last_price = closing_prices[i];
        }
        return 100 - 100 / (1 + up_ewma / down_ewma);
    }

    //Simple moving average
    inline double sma(const std::vector<double>& closing_prices, const uint32_t evaluation_size){
        double sum = 0;
        int last_value = closing_prices.size() - evaluation_size;
        for(int i = closing_prices.size() - 1; i >= last_value; --i){
            sum+= closing_prices[i];
        }
        return sum/evaluation_size;
    }

    // raising falling rate 
    inline double rr(double current_price, double last_price){
        return (current_price - last_price) / last_price;
    }

    //Percent difference from the simple moving average in a given period
    inline double pd(double current_price, double sma_on_period){
        return ( (current_price - sma_on_period) / sma_on_period ) * 100;
    }

    inline double pd(const std::vector<double>& closing_prices, const uint32_t evaluation_size){
        double sma_period = sma(closing_prices, evaluation_size);
        return pd(closing_prices[closing_prices.size() - 1], sma_period);
    }
}
#endif
