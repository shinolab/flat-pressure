#pragma once

#include <stdio.h>

#include <algorithm>
#include <iostream>
#include <memory>
#include <new>
#include <string>
#include <tuple>
#include <utility>
#include <vector>
// #include <conio.h>
#include <math.h>

#include <fstream>
#include <random>
#include <ranges>
#include <sstream>

#include "autd3.hpp"
#include "autd3/gain/holo.hpp"

const auto backend = std::make_shared<autd3::gain::holo::NalgebraBackend>();

namespace HandMadeSplit {
std::vector<std::string> split(const std::string& input, char delimiter) {
    std::istringstream stream(input);
    std::string field;
    std::vector<std::string> result;
    while (getline(stream, field, delimiter)) result.push_back(field);
    return result;
}
};  // namespace HandMadeSplit

std::vector<float> read(int range) {
    std::cout << range << "\n";
    std::ostringstream oss;
    oss << range;
    std::string str = oss.str();

    std::string path =
        "/Users/koda/Desktop/autd3-cpp-main2/examples/tests/HoloFlatData/"
        "holo_rad_" +
        str + ".csv";
    std::cout << path << "\n";
    std::ifstream ifs(path);
    std::vector<float> data;
    if (!ifs) {
        std::cout << "error" << std::endl;
    }
    if (ifs) {
        std::string line;

        while (getline(ifs, line)) {
            std::vector<float> datvec;
            std::vector<std::string> strvec = HandMadeSplit::split(line, ',');
            for (auto&& s : strvec) datvec.push_back(std::stof(s));
            // std::cout << datvec[0] << " \n";
            data.push_back(datvec[0]);
        }
    }
    return data;
}

class HoloFlat final : public autd3::Gain<HoloFlat> {
   public:
    explicit HoloFlat(int range) : _range(range), PhaseArray(read(_range)){};
    mutable int i = 0;
    // float PhaseArray[1992] = { 1 };
    // std::vector<float> PhaseArray = read(_range);

    // std::vector<autd3::driver::Drive> calc(const autd3::Geometry& geometry)
    // const override {
    //     std::vector<autd3::driver::Drive> drives;
    //     drives.reserve(geometry.num_transducers());
    //     std::transform(geometry.begin(), geometry.end(),
    //     std::back_inserter(drives), [&](const auto& transducer) {
    //         const auto phase = PhaseArray[i];
    //         i++;
    //         return autd3::Drive{ phase, 1 };
    //         });
    //     return drives;
    // }

    std::unordered_map<size_t, std::vector<autd3::Drive>> calc(
        const autd3::Geometry& geometry) const override {
        return autd3::Gain<HoloFlat>::transform(
            geometry, [&](const auto& dev, const auto& transducer) {
                const auto phase = PhaseArray[i];
                i++;
                return autd3::Drive{autd3::Phase::from_rad(phase),
                                    autd3::EmitIntensity::maximum()};
            });
    }

   private:
    int _range;
    std::vector<float> PhaseArray;
};

using namespace std::ranges::views;







template <typename L>
inline void kishi_test_B4jikken(autd3::Controller<L>& autd) {
    autd.send(autd3::Clear());
    // autd.send(autd3::Synchronize());

    auto silencer = autd3::ConfigureSilencer::disable();
    // auto silencer = autd3::ConfigureSilencer::silencer(400, 4096);
    autd.send(silencer);

    autd3::modulation::Static m;
    // autd3::modulation::Sine m(150);

    const autd3::Vector3 center =
     autd.geometry().center() + autd3::Vector3(0.0, 0.0, 233.0);

    // 文字の定義
    autd3::Vector3 focus;
    std::string s, t, key, ch, width_str, freq_str, division_str;
    std::string w = "none";
    int points_num, total_points_num, r, l;
    float interval, x, y;

    // パラメータの指定
    // t = 焦点移動パターン, s = 1辺の長さ, key = 強さの調整

    std::cout << "type(0, 1, or 2) : "; // "type" と表示
    std::getline(std::cin, t); // ユーザの入力をもらってくる。

    std::cout << "division (1, 2, or 3) : ";
    std::getline(std::cin, division_str);

    //std::cout << "size (20, 30, 40, or 50) : ";
    //std::getline(std::cin, s);

    //std::cout << "freq (100 or 200) : ";
    //std::getline(std::cin, freq_str);

    //float range = std::stof(s);
    float range = 40; //今回は固定値。
    //float freq = std::stof(freq_str);
    float freq = 5; //今回は固定値。
    float division = std::stof(division_str);


    // type == 0 : パターン(a)(b)
    // 系統。折れる長さをもらう。



    if (t == "0" && division == 1) {
        int amp = 128;
        //std::cout << "width (5 or 10) : ";
        //std::getline(std::cin, width_str);
        //float width = std::stof(width_str);
        float width = 10;

        const size_t total_points_num = 5000 / freq; // 焦点数
        const float interval = range * (range / width + 1 + 1) / (total_points_num - 1);
        // 焦点の総移動距離を「焦点-1」で割る（ = 焦点間距離）
        const size_t points_num = (range + width) / interval;

        auto stm = autd3::FocusSTM::from_freq(freq).add_foci_from_iter(
            iota(0) | take(total_points_num) | transform([&](auto i) {
                r = i / points_num;
                l = i % points_num;

                if (r % 2 == 0) {
                    if (l < range / interval) {
                        x = l * interval;
                        y = r * width;
                    } else {
                        x = range;
                        y = width * r + (l - range / interval) * interval;
                    }
                } else {
                    if (l < range / interval) {
                        x = range - l * interval;
                        y = r * width;
                    } else {
                        x = 0;
                        y = width * r + (l - range / interval) * interval;
                    }
                }
                autd3::Vector3 p = center + autd3::Vector3(x, y, 0);
                return p;
            }));

        std::cout << "Actual frequency is " << stm.frequency() << " Hz\n";
        //autd.send(m, stm);



            //まずは参照パターンを提示する。
            HoloFlat g(range);
            autd3::modulation::Sine m(150);
            autd.send(m, g, std::chrono::milliseconds(200));

            std::cout << amp << " \n";

            std::cout << "Are you ready?: ";
            std::string ready;
            getline(std::cin, ready);

            //ここから、二分探索。
            int left = 0;
            int right = 256; //本当は255だと思うけれど、計算の簡単のため。

            while((right-left) > 2){

                std::cout << amp << " \n";

                autd3::modulation::Static m_stm(amp);
                autd.send(m_stm, stm, std::chrono::milliseconds(200));//最初は128

                std::cout << "key: ";
                getline(std::cin, key);

                int mid = (left + right)/2;
                //この時点で amp = mid になっているはず。

                if(key == "@"){//弱かったら、強める。
                    amp += (mid - left)/2;
                    left = mid;
                } else if (key == ":") {//強かったら、弱める。
                    amp -= (mid - left)/2;
                    right = mid;
                }  else if (key == "-") {//同じ周波数
                    continue;
                } else if (key == ";") {//参照パターンみる
                    HoloFlat g(range);
                    autd3::modulation::Sine m(150);
                    autd.send(m, g, std::chrono::milliseconds(200));

                    std::cout << "Are you ready?: ";
                    getline(std::cin, ready);
                } else if (key == "q") {
                    break;
                }

                if ((right-left) == 4){
                    std::cout << "Last: ";
                }


            }
            std::cout << amp << " \n";
            autd3::modulation::Static m_stm(amp);
            autd.send(m_stm, stm, std::chrono::milliseconds(200));//最初は128
    }




    else if (t == "0" && division == 2) {
        int amp = 128;
        //std::cout << "width (5 or 10) : ";
        //std::getline(std::cin, width_str);
        //float width = std::stof(width_str) / 2;
        float width = 5 / 2;


        const size_t total_points_num = 5000 / freq;
        const float interval =
            range * (range / width + 1 + 1) / (total_points_num - 1) / 2;
        const size_t points_num = (range + width) / interval;


            //まずは参照パターンを提示する。
            HoloFlat g(range);
            autd3::modulation::Sine m(150);
            autd.send(m, g, std::chrono::milliseconds(200));

            std::cout << amp << " \n";

            std::cout << "Are you ready?: ";
            std::string ready;
            getline(std::cin, ready);


            int left = 0;
            int right = 256;

        while((right-left) > 2){
            auto stm = autd3::GainSTM::from_freq(freq).add_gains_from_iter(
            iota(0) | take(total_points_num) | transform([&](auto i) {
                r = i / points_num;
                l = i % points_num;

                if (r % 2 == 0) {
                    if (l < range / 2 / interval) {
                        x = l * interval;
                        y = r * width;
                    } else {
                        x = range / 2;
                        y = width * r + (l - range / interval / 2) * interval;
                    }
                } else {
                    if (l < range / 2 / interval) {
                        x = range / 2 - l * interval;
                        y = r * width;
                    } else {
                        x = 0;
                        y = width * r + (l - range / 2 / interval) * interval;
                    }
                }

                autd3::Vector3 p = center + autd3::Vector3(x, y, 0);
                autd3::Vector3 q = p + autd3::Vector3(range / 2, 0, 0);
                autd3::Vector3 r = p + autd3::Vector3(0, range / 2, 0);
                autd3::Vector3 s = p + autd3::Vector3(range / 2, range / 2, 0);

                using autd3::gain::holo::Pascal;
                auto g = autd3::gain::holo::Naive(backend) //LSS
                             .add_focus(p, 5e5 * Pascal)
                             .add_focus(q, 5e5 * Pascal)
                             .add_focus(r, 5e5 * Pascal)
                             .add_focus(s, 5e5 * Pascal);
                return g;
            }));

                //std::cout << "Actual frequency is " << stm.frequency() << " Hz\n";
                std::cout << amp << " \n";

                autd3::modulation::Static m_stm(amp);
                autd.send(m_stm, stm, std::chrono::milliseconds(200));

                std::cout << "key: ";
                getline(std::cin, key);

                int mid = (left + right)/2;
                if(key == "@"){
                    amp += (mid - left)/2;
                    left = mid;
                } else if (key == ":") {
                    amp -= (mid - left)/2;
                    right = mid;
                } else if (key == ";") {
                    HoloFlat g(range);
                    autd3::modulation::Sine m(150);
                    autd.send(m, g, std::chrono::milliseconds(200));

                    std::cout << "Are you ready?: ";
                    getline(std::cin, ready);
                } else if (key == "-") {//同じ周波数
                    continue;
                                } else if (key == "q") {
                    break;
                }

                if ((right-left) == 4){
                    std::cout << "Last: ";
                }
            }

        //std::cout << "press enter to finish..." << std::endl;
        //std::cin.ignore();
        std::cout << amp << " \n";
            auto stm = autd3::GainSTM::from_freq(freq).add_gains_from_iter(
            iota(0) | take(total_points_num) | transform([&](auto i) {
                r = i / points_num;
                l = i % points_num;

                if (r % 2 == 0) {
                    if (l < range / 2 / interval) {
                        x = l * interval;
                        y = r * width;
                    } else {
                        x = range / 2;
                        y = width * r + (l - range / interval / 2) * interval;
                    }
                } else {
                    if (l < range / 2 / interval) {
                        x = range / 2 - l * interval;
                        y = r * width;
                    } else {
                        x = 0;
                        y = width * r + (l - range / 2 / interval) * interval;
                    }
                }

                autd3::Vector3 p = center + autd3::Vector3(x, y, 0);
                autd3::Vector3 q = p + autd3::Vector3(range / 2, 0, 0);
                autd3::Vector3 r = p + autd3::Vector3(0, range / 2, 0);
                autd3::Vector3 s = p + autd3::Vector3(range / 2, range / 2, 0);

                using autd3::gain::holo::Pascal;
                auto g = autd3::gain::holo::Naive(backend) //LSS
                             .add_focus(p, 5e5 * Pascal)
                             .add_focus(q, 5e5 * Pascal)
                             .add_focus(r, 5e5 * Pascal)
                             .add_focus(s, 5e5 * Pascal);
                return g;
            }));
        autd3::modulation::Static m_stm(amp);
        autd.send(m_stm, stm, std::chrono::milliseconds(200));//最初は128
    }



    else if (t == "0" && division == 3) {
        int amp = 128;
        //std::cout << "width (5 or 10) : ";
        //std::getline(std::cin, width_str);
        //float width = std::stof(width_str) / 3;
        float width = 5 / 3;

        const size_t total_points_num = 5000 / freq;
        const float interval =
            range * (range / width + 1 + 1) / (total_points_num - 1) / 3;
        const size_t points_num = (range + width) / interval;

        //まずは参照パターンを提示する。
            HoloFlat g(range);
            autd3::modulation::Sine m(150);
            autd.send(m, g, std::chrono::milliseconds(200));

            std::cout << amp << " \n";

            std::cout << "Are you ready?: ";
            std::string ready;
            getline(std::cin, ready);


            int left = 0;
            int right = 256;

        while((right-left) > 2){
        auto stm = autd3::GainSTM::from_freq(freq).add_gains_from_iter(
            iota(0) | take(total_points_num) | transform([&](auto i) {
                r = i / points_num;
                l = i % points_num;

                if (r % 2 == 0) {
                    if (l < range / 3 / interval) {
                        x = l * interval;
                        y = r * width;
                    } else {
                        x = range / 3;
                        y = width * r + (l - range / interval / 3) * interval;
                    }
                } else {
                    if (l < range / 3 / interval) {
                        x = range / 3 - l * interval;
                        y = r * width;
                    } else {
                        x = 0;
                        y = width * r + (l - range / 3 / interval) * interval;
                    }
                }
                autd3::Vector3 p = center + autd3::Vector3(x, y, 0);
                autd3::Vector3 q = p + autd3::Vector3(range / 3, 0, 0);
                autd3::Vector3 r = p + autd3::Vector3(2 * range / 3, 0, 0);
                autd3::Vector3 s = p + autd3::Vector3(0, range / 3, 0);
                autd3::Vector3 t = center + autd3::Vector3(0, 2 * range / 3, 0);
                autd3::Vector3 u = p + autd3::Vector3(range / 3, range / 3, 0);
                autd3::Vector3 v =
                    p + autd3::Vector3(range / 3, 2 * range / 3, 0);
                autd3::Vector3 w =
                    p + autd3::Vector3(2 * range / 3, range / 3, 0);
                autd3::Vector3 x =
                    p + autd3::Vector3(2 * range / 3, 2 * range / 3, 0);

                using autd3::gain::holo::Pascal;
                auto g = autd3::gain::holo::Naive(backend)
                             .add_focus(p, 5e5 * Pascal)
                             .add_focus(q, 5e5 * Pascal)
                             .add_focus(r, 5e5 * Pascal)
                             .add_focus(s, 5e5 * Pascal)
                             .add_focus(t, 5e5 * Pascal)
                             .add_focus(u, 5e5 * Pascal)
                             .add_focus(v, 5e5 * Pascal)
                             .add_focus(w, 5e5 * Pascal)
                             .add_focus(x, 5e5 * Pascal);
                return g;
            }));


                //std::cout << "Actual frequency is " << stm.frequency() << " Hz\n";
                std::cout << amp << " \n";

                autd3::modulation::Static m_stm(amp);
                autd.send(m_stm, stm, std::chrono::milliseconds(200));//最初は128

                std::cout << "key: ";
                getline(std::cin, key);

                int mid = (left + right)/2;
                //この時点で amp = mid になっているはず。

                if(key == "@"){//弱かったら、強める。
                    amp += (mid - left)/2;
                    left = mid;
                } else if (key == ":") {//強かったら、弱める。
                    amp -= (mid - left)/2;
                    right = mid;
                } else if (key == ";") {//参照パターンみる
                    HoloFlat g(range);
                    autd3::modulation::Sine m(150);
                    autd.send(m, g, std::chrono::milliseconds(200));

                    std::cout << "Are you ready?: ";
                    getline(std::cin, ready);
                } else if (key == "-") {//同じ周波数
                    continue;
                                } else if (key == "q") {
                    break;
                }

                if ((right-left) == 4){
                    std::cout << "Last: ";
                }
        }
        std::cout << amp << " \n";

        auto stm = autd3::GainSTM::from_freq(freq).add_gains_from_iter(
            iota(0) | take(total_points_num) | transform([&](auto i) {
                r = i / points_num;
                l = i % points_num;

                if (r % 2 == 0) {
                    if (l < range / 3 / interval) {
                        x = l * interval;
                        y = r * width;
                    } else {
                        x = range / 3;
                        y = width * r + (l - range / interval / 3) * interval;
                    }
                } else {
                    if (l < range / 3 / interval) {
                        x = range / 3 - l * interval;
                        y = r * width;
                    } else {
                        x = 0;
                        y = width * r + (l - range / 3 / interval) * interval;
                    }
                }
                autd3::Vector3 p = center + autd3::Vector3(x, y, 0);
                autd3::Vector3 q = p + autd3::Vector3(range / 3, 0, 0);
                autd3::Vector3 r = p + autd3::Vector3(2 * range / 3, 0, 0);
                autd3::Vector3 s = p + autd3::Vector3(0, range / 3, 0);
                autd3::Vector3 t = center + autd3::Vector3(0, 2 * range / 3, 0);
                autd3::Vector3 u = p + autd3::Vector3(range / 3, range / 3, 0);
                autd3::Vector3 v =
                    p + autd3::Vector3(range / 3, 2 * range / 3, 0);
                autd3::Vector3 w =
                    p + autd3::Vector3(2 * range / 3, range / 3, 0);
                autd3::Vector3 x =
                    p + autd3::Vector3(2 * range / 3, 2 * range / 3, 0);

                using autd3::gain::holo::Pascal;
                auto g = autd3::gain::holo::Naive(backend)
                             .add_focus(p, 5e5 * Pascal)
                             .add_focus(q, 5e5 * Pascal)
                             .add_focus(r, 5e5 * Pascal)
                             .add_focus(s, 5e5 * Pascal)
                             .add_focus(t, 5e5 * Pascal)
                             .add_focus(u, 5e5 * Pascal)
                             .add_focus(v, 5e5 * Pascal)
                             .add_focus(w, 5e5 * Pascal)
                             .add_focus(x, 5e5 * Pascal);
                return g;
            }));
            autd3::modulation::Static m_stm(amp);
            autd.send(m_stm, stm, std::chrono::milliseconds(200));//最初は128
    }



    // type == 1 : パターン(c)
    else if (t == "1" && division == 1) {
        int amp = 128;
        const size_t total_points_num = 5000 / freq;
        const float interval = range / (total_points_num / 4);
        const size_t points_num = range / interval;


        auto stm = autd3::FocusSTM::from_freq(freq).add_foci_from_iter(
            iota(0) | take(total_points_num) | transform([&](auto i) {
                r = i / points_num;
                l = i % points_num;

                if (r == 0) {
                    x = l * interval;
                    y = 0;
                } else if (r == 1) {
                    x = range;
                    y = l * interval;
                } else if (r == 2) {
                    x = range - l * interval;
                    y = range;
                } else {
                    x = 0;
                    y = range - l * interval;
                }

                autd3::Vector3 p = center + autd3::Vector3(x, y, 0);
                return p;
            }));

        std::cout << "Actual frequency is " << stm.frequency() << " Hz\n";
        //autd.send(m, stm);



            //まずは参照パターンを提示する。
            HoloFlat g(range);
            autd3::modulation::Sine m(150);
            autd.send(m, g, std::chrono::milliseconds(200));

            std::cout << amp << " \n";

            std::cout << "Are you ready?: ";
            std::string ready;
            getline(std::cin, ready);

            //ここから、二分探索。
            int left = 0;
            int right = 256; //本当は255だと思うけれど、計算の簡単のため。

            while((right-left) > 2){

                std::cout << amp << " \n";

                autd3::modulation::Static m_stm(amp);
                autd.send(m_stm, stm, std::chrono::milliseconds(200));//最初は128

                std::cout << "key: ";
                getline(std::cin, key);

                int mid = (left + right)/2;
                //この時点で amp = mid になっているはず。

                if(key == "@"){//弱かったら、強める。
                    amp += (mid - left)/2;
                    left = mid;
                } else if (key == ":") {//強かったら、弱める。
                    amp -= (mid - left)/2;
                    right = mid;
                } else if (key == "-") {//同じ周波数
                    continue;
                } else if (key == ";") {//参照パターンみる
                    HoloFlat g(range);
                    autd3::modulation::Sine m(150);
                    autd.send(m, g, std::chrono::milliseconds(200));

                    std::cout << "Are you ready?: ";
                    getline(std::cin, ready);
                                } else if (key == "q") {
                    break;
                }

                if ((right-left) == 4){
                    std::cout << "Last: ";
                }
            }
            std::cout << amp << " \n";
            autd3::modulation::Static m_stm(amp);
            autd.send(m_stm, stm, std::chrono::milliseconds(200));//最初は128
    }




    else if (t == "1" && division == 2) {
        int amp = 128;
        const size_t total_points_num = 5000 / freq;
        const float interval = range / 2 / (total_points_num / 4);
        const size_t points_num = range / 2 / interval;

        //まずは参照パターンを提示する。
            HoloFlat g(range);
            autd3::modulation::Sine m(150);
            autd.send(m, g, std::chrono::milliseconds(200));

            std::cout << amp << " \n";

            std::cout << "Are you ready?: ";
            std::string ready;
            getline(std::cin, ready);


            int left = 0;
            int right = 256;

        while((right-left) > 2){
        auto stm = autd3::GainSTM::from_freq(freq).add_gains_from_iter(
            iota(0) | take(total_points_num) | transform([&](auto i) {
                r = i / points_num;
                l = i % points_num;

                if (r == 0) {
                    x = l * interval;
                    y = 0;
                } else if (r == 1) {
                    x = range;
                    y = l * interval;
                } else if (r == 2) {
                    x = range - l * interval;
                    y = range;
                } else {
                    x = 0;
                    y = range - l * interval;
                }

                autd3::Vector3 p = center + autd3::Vector3(x, y, 0);
                autd3::Vector3 q = p + autd3::Vector3(range / 2, 0, 0);
                autd3::Vector3 r = p + autd3::Vector3(0, range / 2, 0);
                autd3::Vector3 s = p + autd3::Vector3(range / 2, range / 2, 0);

                using autd3::gain::holo::Pascal;

                auto g = autd3::gain::holo::Naive(backend)
                             .add_focus(p, 5e5 * Pascal)
                             .add_focus(q, 5e5 * Pascal)
                             .add_focus(r, 5e5 * Pascal)
                             .add_focus(s, 5e5 * Pascal);
                return g;
            }));

                std::cout << "Actual frequency is " << stm.frequency() << " Hz\n";

                std::cout << amp << " \n";

                autd3::modulation::Static m_stm(amp);
                autd.send(m_stm, stm, std::chrono::milliseconds(200));//最初は128

                std::cout << "key: ";
                getline(std::cin, key);

                int mid = (left + right)/2;
                //この時点で amp = mid になっているはず。

                if(key == "@"){//弱かったら、強める。
                    amp += (mid - left)/2;
                    left = mid;
                } else if (key == ":") {//強かったら、弱める。
                    amp -= (mid - left)/2;
                    right = mid;
                } else if (key == "-") {//同じ周波数
                    continue;
                } else if (key == ";") {//参照パターンみる
                    HoloFlat g(range);
                    autd3::modulation::Sine m(150);
                    autd.send(m, g, std::chrono::milliseconds(200));

                    std::cout << "Are you ready?: ";
                    getline(std::cin, ready);
                                } else if (key == "q") {
                    break;
                }

                if ((right-left) == 4){
                    std::cout << "Last: ";
                }
        }
        std::cout << amp << " \n";

        auto stm = autd3::GainSTM::from_freq(freq).add_gains_from_iter(
            iota(0) | take(total_points_num) | transform([&](auto i) {
                r = i / points_num;
                l = i % points_num;

                if (r == 0) {
                    x = l * interval;
                    y = 0;
                } else if (r == 1) {
                    x = range;
                    y = l * interval;
                } else if (r == 2) {
                    x = range - l * interval;
                    y = range;
                } else {
                    x = 0;
                    y = range - l * interval;
                }

                autd3::Vector3 p = center + autd3::Vector3(x, y, 0);
                autd3::Vector3 q = p + autd3::Vector3(range / 2, 0, 0);
                autd3::Vector3 r = p + autd3::Vector3(0, range / 2, 0);
                autd3::Vector3 s = p + autd3::Vector3(range / 2, range / 2, 0);

                using autd3::gain::holo::Pascal;

                auto g = autd3::gain::holo::Naive(backend)
                             .add_focus(p, 5e5 * Pascal)
                             .add_focus(q, 5e5 * Pascal)
                             .add_focus(r, 5e5 * Pascal)
                             .add_focus(s, 5e5 * Pascal);
                return g;
            }));

        autd3::modulation::Static m_stm(amp);
        autd.send(m_stm, stm, std::chrono::milliseconds(200));//最初は128
    }



    else if (t == "1" && division == 3) {
        int amp = 128;
        const size_t total_points_num = 5000 / freq;
        const float interval = range / 3 / (total_points_num / 4);
        const size_t points_num = range / 3 / interval;

        //まずは参照パターンを提示する。
            HoloFlat g(range);
            autd3::modulation::Sine m(150);
            autd.send(m, g, std::chrono::milliseconds(200));

            std::cout << amp << " \n";

            std::cout << "Are you ready?: ";
            std::string ready;
            getline(std::cin, ready);


            int left = 0;
            int right = 256;

        while((right-left) > 2){
        auto stm = autd3::GainSTM::from_freq(freq).add_gains_from_iter(
            iota(0) | take(total_points_num) | transform([&](auto i) {
                r = i / points_num;
                l = i % points_num;

                if (r == 0) {
                    x = l * interval;
                    y = 0;
                } else if (r == 1) {
                    x = range;
                    y = l * interval;
                } else if (r == 2) {
                    x = range - l * interval;
                    y = range;
                } else {
                    x = 0;
                    y = range - l * interval;
                }

                autd3::Vector3 p = center + autd3::Vector3(x, y, 0);
                autd3::Vector3 q = p + autd3::Vector3(range / 3, 0, 0);
                autd3::Vector3 r = p + autd3::Vector3(2 * range / 3, 0, 0);
                autd3::Vector3 s = p + autd3::Vector3(0, range / 3, 0);
                autd3::Vector3 t = center + autd3::Vector3(0, 2 * range / 3, 0);
                autd3::Vector3 u = p + autd3::Vector3(range / 3, range / 3, 0);
                autd3::Vector3 v =
                    p + autd3::Vector3(range / 3, 2 * range / 3, 0);
                autd3::Vector3 w =
                    p + autd3::Vector3(2 * range / 3, range / 3, 0);
                autd3::Vector3 x =
                    p + autd3::Vector3(2 * range / 3, 2 * range / 3, 0);

                using autd3::gain::holo::Pascal;

                auto g = autd3::gain::holo::Naive(backend)
                             .add_focus(p, 5e5 * Pascal)
                             .add_focus(q, 5e5 * Pascal)
                             .add_focus(r, 5e5 * Pascal)
                             .add_focus(s, 5e5 * Pascal)
                             .add_focus(t, 5e5 * Pascal)
                             .add_focus(u, 5e5 * Pascal)
                             .add_focus(v, 5e5 * Pascal)
                             .add_focus(w, 5e5 * Pascal)
                             .add_focus(x, 5e5 * Pascal);
                return g;
            }));


                std::cout << "Actual frequency is " << stm.frequency() << " Hz\n";

                std::cout << amp << " \n";

                autd3::modulation::Static m_stm(amp);
                autd.send(m_stm, stm, std::chrono::milliseconds(200));//最初は128

                std::cout << "key: ";
                getline(std::cin, key);

                int mid = (left + right)/2;

                if(key == "@"){
                    amp += (mid - left)/2;
                    left = mid;
                } else if (key == ":") {
                    amp -= (mid - left)/2;
                    right = mid;
                } else if (key == "-") {//同じ周波数
                    continue;
                } else if (key == ";") {
                    HoloFlat g(range);
                    autd3::modulation::Sine m(150);
                    autd.send(m, g, std::chrono::milliseconds(200));

                    std::cout << "Are you ready?: ";
                    getline(std::cin, ready);
                                } else if (key == "q") {
                    break;
                }

                if ((right-left) == 4){
                    std::cout << "Last: ";
                }
        }
        std::cout << amp << " \n";

        auto stm = autd3::GainSTM::from_freq(freq).add_gains_from_iter(
            iota(0) | take(total_points_num) | transform([&](auto i) {
                r = i / points_num;
                l = i % points_num;

                if (r == 0) {
                    x = l * interval;
                    y = 0;
                } else if (r == 1) {
                    x = range;
                    y = l * interval;
                } else if (r == 2) {
                    x = range - l * interval;
                    y = range;
                } else {
                    x = 0;
                    y = range - l * interval;
                }

                autd3::Vector3 p = center + autd3::Vector3(x, y, 0);
                autd3::Vector3 q = p + autd3::Vector3(range / 3, 0, 0);
                autd3::Vector3 r = p + autd3::Vector3(2 * range / 3, 0, 0);
                autd3::Vector3 s = p + autd3::Vector3(0, range / 3, 0);
                autd3::Vector3 t = center + autd3::Vector3(0, 2 * range / 3, 0);
                autd3::Vector3 u = p + autd3::Vector3(range / 3, range / 3, 0);
                autd3::Vector3 v =
                    p + autd3::Vector3(range / 3, 2 * range / 3, 0);
                autd3::Vector3 w =
                    p + autd3::Vector3(2 * range / 3, range / 3, 0);
                autd3::Vector3 x =
                    p + autd3::Vector3(2 * range / 3, 2 * range / 3, 0);

                using autd3::gain::holo::Pascal;

                auto g = autd3::gain::holo::Naive(backend)
                             .add_focus(p, 5e5 * Pascal)
                             .add_focus(q, 5e5 * Pascal)
                             .add_focus(r, 5e5 * Pascal)
                             .add_focus(s, 5e5 * Pascal)
                             .add_focus(t, 5e5 * Pascal)
                             .add_focus(u, 5e5 * Pascal)
                             .add_focus(v, 5e5 * Pascal)
                             .add_focus(w, 5e5 * Pascal)
                             .add_focus(x, 5e5 * Pascal);
                return g;
            }));


        autd3::modulation::Static m_stm(amp);
        autd.send(m_stm, stm, std::chrono::milliseconds(200));//最初は128
    }
}
