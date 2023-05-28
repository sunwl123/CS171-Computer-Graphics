#include "utils.h"

bool is_prime(int n)
{
    // judge if the number is a prime
    for (int i = 2; i * i <= n; ++i)
    {
        if (n % i == 0)
        {
            return false;
        }
    }
    return n > 1;
}

vector<string> Utils::split(char *str)
{
    // split the str by space or \n or \r
    std::vector<std::string> res;
    std::string cur = "";
    for (int i = 0; str[i]; ++i)
    {
        if (str[i] == ' ' || str[i] == '\n' || str[i] == '\r')
        {
            if (cur != "")
            {
                res.push_back(cur);
                cur = "";
            }
        }
        else
        {
            cur += str[i];
        }
    }
    if (cur != "")
    {
        res.push_back(cur);
    }
    return res;
}

double Utils::randomQMC(int axis, long long step)
{
    // generate a random number using QMC
    int base = prime[axis];
    double res = 0.0;
    double fraction = 1.0;
    while (step > 0)
    {
        fraction /= base;
        res += fraction * (step % base);
        step /= base;
    }
    return res;
}

static double Utils::random_double()
{
    // generate random double number
    static std::mt19937 *generator = nullptr;
    if (generator == nullptr)
    {
        generator = new std::mt19937(clock());
    }
    static std::uniform_real_distribution<> dis(0, 1);
    return dis(*generator);
}

double Utils::my_random(double min, double max, int axis, long long step)
{
    // generate random double number in [min, max]
    if (axis == -1)
    {
        return min + random_double() * (max - min);
    }
    return min + randomQMC(axis, step) * (max - min);
}