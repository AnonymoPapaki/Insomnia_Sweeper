#pragma once

#define ArrayCount(Array) (sizeof(Array) / sizeof((Array)[0]))
#define Absolute(Value) (((Value) < 0)?-(Value):(Value))

#define Kilobytes(Value) ((Value)*1024)
#define Megabytes(Value) (Kilobytes(Value)*1024)
#define Gigabytes(Value) (Megabytes(Value)*1024LL)
#define Terrabytes(Value) (Gigabytes(Value)*1024LL)
