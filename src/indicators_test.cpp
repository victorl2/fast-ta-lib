#include "indicators.hpp"
#include <assert.h>

#define MAX_ERROR 0.0001

void rsi_sma_value_test(const std::vector<double>& closing_prices, const uint32_t evaluation_size){
    std::cout << "+ Testing rsi using simple moving average calculation" << std::endl;
    double rsi_sma = fta::rsi_sma(closing_prices, evaluation_size);
    assert(rsi_sma - 51.23274789 < MAX_ERROR);
}

void sma_value_test(const std::vector<double>& closing_prices, const uint32_t evaluation_size){
    std::cout << "+ Testing simple moving average calculation" << std::endl;
    double sma_result = fta::sma(closing_prices, evaluation_size);
    assert(sma_result - 19125.50714285 < MAX_ERROR);
}

void rsi_ewma_value_test(const std::vector<double>& closing_prices, const uint32_t evaluation_size, double alpha){
    std::cout << "+ Testing rsi using exponential weighted moving average calculation" << std::endl;
    double rsi_ewnma = fta::rsi_ewma(closing_prices, evaluation_size, alpha);
    assert(rsi_ewnma - 51.08199602 < MAX_ERROR);
}

void ewma_value_test(const std::vector<double>& closing_prices, const uint32_t evaluation_size, double alpha){
    std::cout << "+ Testing exponential weighted moving average calculation" << std::endl;
    double ewma = fta::ewma(closing_prices, evaluation_size, alpha);
    assert(ewma - 19009.88973715 < MAX_ERROR);
}

void rr_value_test(const double current_price, const double last_price){
    std::cout << "+ Testing rr calculation" << std::endl;
    double rr = fta::rr(current_price, last_price);
    assert(rr - 0.00845444 < MAX_ERROR);
}

void pd_value_test(const std::vector<double>& closing_prices, const uint32_t evaluation_size){
    std::cout << "+ Testing percent difference calculation" << std::endl;
    
    double sma_period = fta::sma(closing_prices, evaluation_size);
    double pd = fta::pd(closing_prices[closing_prices.size() - 1], sma_period);
    
    assert(pd - 0.84544409 < MAX_ERROR);
    assert(pd == fta::pd(closing_prices, evaluation_size));
}

int main(){
    std::cout << "##Testing Tecnical Analyses Indicators values" << std::endl << std::endl;

    static const double arr[] = {19789.71, 19760.91, 19345.01, 18573.51, 18724.94, 18762.92, 18921.55};
    std::vector<double> closing_prices(arr, arr + sizeof(arr) / sizeof(arr[0]) );    
    uint32_t evaluation_size = 7;
    
    const double alpha = 0.940;
    double current_price = closing_prices[closing_prices.size() -1];
    double last_price = closing_prices[closing_prices.size() -2];

    sma_value_test(closing_prices, evaluation_size);
    rsi_sma_value_test(closing_prices, evaluation_size);
    rsi_ewma_value_test(closing_prices, evaluation_size, alpha);
    ewma_value_test(closing_prices, evaluation_size, alpha);
    rr_value_test(current_price, last_price);
    pd_value_test(closing_prices, evaluation_size);

    std::cout << std::endl << "All tests passed" << std::endl;
}