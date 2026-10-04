#pragma once
#include <cstdint>
namespace gtanet {
struct WeaponRule { float damage; float range; uint64_t fireIntervalMs; };
inline WeaponRule Weapon(uint8_t w){
    switch(w){
        case 22:return {25.f,70.f,220}; case 23:return {35.f,70.f,280}; case 24:return {46.f,80.f,450};
        case 25:return {40.f,55.f,850}; case 26:return {20.f,45.f,180}; case 27:return {28.f,60.f,450};
        case 28:return {8.f,75.f,75}; case 29:return {9.f,90.f,85}; case 30:return {10.f,130.f,95};
        case 31:return {10.f,150.f,90}; case 32:return {8.f,80.f,70}; case 33:return {55.f,180.f,700};
        case 34:return {100.f,350.f,1100}; default:return {15.f,25.f,350};
    }
}
}