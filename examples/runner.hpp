#pragma once

#include <algorithm>
#include <functional>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "autd3.hpp"
#include "tests/advanced.hpp"
#include "tests/bessel.hpp"
#include "tests/flag.hpp"
#include "tests/focus.hpp"
#include "tests/group.hpp"
#include "tests/holo.hpp"
#include "tests/mod_audio_file.hpp"
#include "tests/plane.hpp"
#include "tests/stm.hpp"
#include "tests/transtest.hpp"
//#include "tests/kishi_new_first.hpp" //ここ足しました！！
#include "tests/kishi_new_b4jikken.hpp" //ここ足しました！！

template <typename L>
inline int run(autd3::Controller<L>& autd) {
    using F = std::function<void(autd3::Controller<L>&)>;
    std::vector<std::pair<F, std::string>> tests = {
        std::pair(F{focus_test<L>}, "Single focus test"),
        std::pair(F{bessel_test<L>}, "Bessel beam test"),
        std::pair(F{plane_test<L>}, "Plane wave test"),
        std::pair(F{mod_audio_file_test<L>}, "Wav modulation test"),
        std::pair(F{focus_stm<L>}, "FocusSTM test"),
        std::pair(F{gain_stm<L>}, "GainSTM test"),
        std::pair(F{holo_test<L>}, "Multiple foci test"),
        std::pair(F{advanced_test<L>}, "Custom Gain & Modulation test"),
        std::pair(F{flag_test<L>}, "Flag test"),
        std::pair(F{tran_test<L>}, "TransducerTest test"),
        std::pair(F{group_by_transducer_test<L>},
                  "Group (by Transducer) test"),
        //std::pair(F{kishi_test_focus_stm<L>},
                  //"kishi_test_focus_stm"),
        std::pair(F{kishi_test_B4jikken<L>}, "kishi_test_B4jikken")
    };



    if (autd.geometry().num_devices() >= 2)
        tests.emplace_back(F{group_by_device_test<L>},
                           "Group (by Device) test");

    const auto firm_infos = autd.firmware_infos();
    std::cout << "======== AUTD3 firmware information ========" << std::endl;
    std::copy(firm_infos.begin(), firm_infos.end(),
              std::ostream_iterator<autd3::FirmwareInfo>(std::cout, "\n"));
    std::cout << "============================================" << std::endl;

    while (true) {
        size_t i = 0;
        std::transform(tests.begin(), tests.end(),
                       std::ostream_iterator<std::string>(std::cout, "\n"),
                       [&i](const auto& test) {
                           return "[" + std::to_string(i++) +
                                  "]: " + test.second;
                       });
        std::cout << "[Others]: finish." << std::endl;

        std::cout << "Choose number: ";
        std::string in;
        size_t idx;
        getline(std::cin, in);
        std::stringstream s(in);
        if (const auto is_empty = in == "\n";
            !(s >> idx) || idx >= tests.size() || is_empty)
            break;

        tests[idx].first(autd);

        std::cout << "press any key to finish..." << std::endl;
        std::cin.ignore();

        std::cout << "finish." << std::endl;
        autd.send(autd3::gain::Null(), autd3::ConfigureSilencer::default_());
    }

    autd.close();

    0;
}
