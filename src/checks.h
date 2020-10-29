//
// Created by arti1208 on 29.10.2020.
//

#ifndef ISP_DZ4_CHECKS_H
#define ISP_DZ4_CHECKS_H

#include <cmath>

const double EPSILON = 1e-06;

bool isZero(double value) {
    return std::abs(value) < EPSILON;
}

#endif //ISP_DZ4_CHECKS_H
