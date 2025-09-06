#pragma once
#include <qfuture.h>

template <typename T> using PromiseLike = std::variant<T, QFuture<T>>;
