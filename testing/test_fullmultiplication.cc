#include "test.hh"
#include <random>

static void multiply(const std::uint32_t* lhs, std::size_t size1,
                     const std::uint32_t* rhs, std::size_t size2,
                     std::uint32_t* result, std::uint32_t* tempBuffer)
{
    const std::size_t resultSize = size1 + size2;
    for(std::size_t i = 0; i < resultSize; ++i) result[i] = 0;

    for(std::size_t lhsInd = size1, resultStartInd = resultSize - 1; lhsInd-- > 0; --resultStartInd)
    {
        for(std::size_t i = size2 + 1; i-- > 0;) tempBuffer[i] = 0;

        std::uint64_t lhsValue = static_cast<std::uint64_t>(lhs[lhsInd]);

        for(std::size_t rhsInd = size2; rhsInd > 0; --rhsInd)
        {
            std::uint64_t product =
                static_cast<std::uint64_t>(rhs[rhsInd-1]) * lhsValue;
            product += tempBuffer[rhsInd];
            tempBuffer[rhsInd] = static_cast<std::uint32_t>(product);
            tempBuffer[rhsInd-1] = static_cast<std::uint32_t>(product>>32);
        }

        std::uint64_t tempVal = 0;
        for(std::size_t rhsInd = size2 + 1, resultInd = resultStartInd; rhsInd-- > 0; --resultInd)
        {
            tempVal +=
                static_cast<std::uint64_t>(tempBuffer[rhsInd]) +
                static_cast<std::uint64_t>(result[resultInd]);
            result[resultInd] = static_cast<std::uint32_t>(tempVal);
            tempVal >>= 32;
        }
    }
}

static bool testTheTest(std::uint32_t lhs, std::uint32_t rhs,
                        std::uint32_t* result, std::uint32_t* tempBuffer)
{
    multiply(&lhs, 1, &rhs, 1, result, tempBuffer);
    const std::uint64_t expectedResult =
        (static_cast<std::uint64_t>(lhs) * static_cast<std::uint64_t>(rhs));
    const std::uint64_t calculatedResult =
        (static_cast<std::uint64_t>(result[0]) << 32) |
        static_cast<std::uint64_t>(result[1]);

    if(expectedResult != calculatedResult)
        return DPRINT("Internal error: Test failed check.\n(lhs=", sethexw0(8), lhs,
                      ", rhs=", sethexw0(8), rhs,
                      ",\n  expected=", sethexw0(16), expectedResult,
                      ",\ncalculated=", sethexw0(16), calculatedResult, ")\n");
    return true;
}

static bool testTheTest(const std::uint32_t* lhs, std::uint32_t rhs,
                        std::uint32_t* result, std::uint32_t* tempBuffer)
{
    multiply(lhs, 2, &rhs, 1, result, tempBuffer);
    const std::uint64_t lhsValue =
        (static_cast<std::uint64_t>(lhs[0]) << 32) | static_cast<std::uint64_t>(lhs[1]);
    const __uint128_t expectedResult =
        (static_cast<__uint128_t>(lhsValue) * static_cast<__uint128_t>(rhs));
    const __uint128_t calculatedResult =
        (static_cast<__uint128_t>(result[0]) << 64) |
        (static_cast<__uint128_t>(result[1]) << 32) |
        static_cast<__uint128_t>(result[2]);

    if(expectedResult != calculatedResult)
        return DPRINT("Internal error: Test failed check.\n(lhs=", sethexw0(16), lhsValue,
                      ", rhs=", sethexw0(8), rhs,
                      ",\n  expected=", sethexw0(32), expectedResult,
                      ",\ncalculated=", sethexw0(32), calculatedResult, ")\n");
    return true;
}

static bool testTheTest(std::uint32_t lhs, const std::uint32_t* rhs,
                        std::uint32_t* result, std::uint32_t* tempBuffer)
{
    multiply(&lhs, 1, rhs, 2, result, tempBuffer);
    const std::uint64_t rhsValue =
        (static_cast<std::uint64_t>(rhs[0]) << 32) | static_cast<std::uint64_t>(rhs[1]);
    const __uint128_t expectedResult =
        (static_cast<__uint128_t>(lhs) * static_cast<__uint128_t>(rhsValue));
    const __uint128_t calculatedResult =
        (static_cast<__uint128_t>(result[0]) << 64) |
        (static_cast<__uint128_t>(result[1]) << 32) |
        static_cast<__uint128_t>(result[2]);

    if(expectedResult != calculatedResult)
        return DPRINT("Internal error: Test failed check.\n(lhs=", sethexw0(8), lhs,
                      ", rhs=", sethexw0(16), rhsValue,
                      ",\n  expected=", sethexw0(32), expectedResult,
                      ",\ncalculated=", sethexw0(32), calculatedResult, ")\n");
    return true;
}

static bool testTheTest(const std::uint32_t* lhs, const std::uint32_t* rhs,
                        std::uint32_t* result, std::uint32_t* tempBuffer)
{
    multiply(lhs, 2, rhs, 2, result, tempBuffer);
    const std::uint64_t lhsValue =
        (static_cast<std::uint64_t>(lhs[0]) << 32) | static_cast<std::uint64_t>(lhs[1]);
    const std::uint64_t rhsValue =
        (static_cast<std::uint64_t>(rhs[0]) << 32) | static_cast<std::uint64_t>(rhs[1]);
    const __uint128_t expectedResult =
        (static_cast<__uint128_t>(lhsValue) * static_cast<__uint128_t>(rhsValue));
    const __uint128_t calculatedResult =
        (static_cast<__uint128_t>(result[0]) << 96) |
        (static_cast<__uint128_t>(result[1]) << 64) |
        (static_cast<__uint128_t>(result[2]) << 32) |
        static_cast<__uint128_t>(result[3]);

    if(expectedResult != calculatedResult)
        return DPRINT("Internal error: Test failed check.\n(lhs=", sethexw0(16), lhsValue,
                      ", rhs=", sethexw0(16), rhsValue,
                      ",\n  expected=", sethexw0(32), expectedResult,
                      ",\ncalculated=", sethexw0(32), calculatedResult, ")\n");
    return true;
}

template<std::size_t kArraySize>
static bool checkResult(const std::uint32_t(&result)[kArraySize],
                        const std::uint32_t(&expectedResult)[kArraySize])
{
    for(std::size_t i = 0; i < kArraySize; ++i)
        if(result[i] != expectedResult[i])
            return DPRINT("Internal error: Test failed check.\n");
    return true;
}

template<std::size_t kSize1, std::size_t kSize2>
static bool testTheTestWithArrays(const std::uint32_t (&lhs)[kSize1],
                                  const std::uint32_t (&rhs)[kSize2],
                                  const std::uint32_t (&expectedResult)[kSize1+kSize2])
{
    std::uint32_t result[kSize1+kSize2], tempBuffer[(kSize1>kSize2?kSize1:kSize2)+1];

    multiply(lhs, kSize1, rhs, kSize2, result, tempBuffer);
    if(!checkResult(result, expectedResult)) DRET;
    multiply(rhs, kSize2, lhs, kSize1, result, tempBuffer);
    if(!checkResult(result, expectedResult)) DRET;
    return true;
}

static bool testTheTest()
{
    std::mt19937_64 rngEngine(123);
    std::uint32_t lhsArray[2], rhsArray[2], resultArray[4], tempBuffer[3];

    for(unsigned iteration = 0; iteration < 100000; ++iteration)
    {
        const std::uint64_t lhs = rngEngine(), rhs = rngEngine();
        lhsArray[0] = static_cast<std::uint32_t>(lhs >> 32);
        lhsArray[1] = static_cast<std::uint32_t>(lhs);
        rhsArray[0] = static_cast<std::uint32_t>(rhs >> 32);
        rhsArray[1] = static_cast<std::uint32_t>(rhs);

        if(!testTheTest(lhsArray[0], rhsArray[0], resultArray, tempBuffer)) DRET;
        if(!testTheTest(lhsArray[1], rhsArray[0], resultArray, tempBuffer)) DRET;
        if(!testTheTest(lhsArray[0], rhsArray[1], resultArray, tempBuffer)) DRET;
        if(!testTheTest(lhsArray[1], rhsArray[1], resultArray, tempBuffer)) DRET;
        if(!testTheTest(lhsArray[0], rhsArray, resultArray, tempBuffer)) DRET;
        if(!testTheTest(lhsArray[1], rhsArray, resultArray, tempBuffer)) DRET;
        if(!testTheTest(lhsArray, rhsArray[0], resultArray, tempBuffer)) DRET;
        if(!testTheTest(lhsArray, rhsArray[1], resultArray, tempBuffer)) DRET;
        if(!testTheTest(lhsArray, rhsArray, resultArray, tempBuffer)) DRET;
    }

    const std::uint32_t lhs1[] =
    { 0xFEDCBA98u, 0x76543210u, 0xFEDCBA98u, 0x76543210u, 0xFEDCBA98u };
    const std::uint32_t rhs1[] =
    { 0xFFEEDDCCu, 0xBBAA9988u, 0x77665544u, 0x332211FFu };
    const std::uint32_t expectedResult1[] =
    { 0xFECBABE3u, 0xB76B4385u, 0x83503068u, 0x3BEFC907u, 0xF6C14789u,
      0x10F4704Du, 0x2C078B73u, 0x7B5EDAB7u, 0x9671F568 };

    const std::uint32_t lhs2[] =
    { 0x50506406u, 0x8282EF7Cu, 0xB6496034u, 0x180233A1u };
    const std::uint32_t rhs2[] =
    { 0xA9ECE60Du, 0x1FFAA7C0u };
    const std::uint32_t expectedResult2[] =
    { 0x354F6450u, 0xDA240DA3, 0x84D34425u, 0x88532514u, 0xEA7D1E4Fu, 0x5B8EBFC0u };

    if(!testTheTestWithArrays(lhs1, rhs1, expectedResult1)) DRET;
    if(!testTheTestWithArrays(lhs2, rhs2, expectedResult2)) DRET;

    return true;
}

template<std::size_t kSize>
static void initWMPUIntWithUint32Array(WMPUInt<kSize>& dest, const std::uint32_t* values)
{
    for(std::size_t i = 0; i < kSize; ++i)
        dest.data()[i] = ((static_cast<std::uint64_t>(values[i*2]) << 32) |
                          static_cast<std::uint64_t>(values[i*2+1]));
}

template<std::size_t kSize1, std::size_t kSize2>
static bool testFullMultiplication(std::mt19937& rng)
{
    const std::size_t kIterations = 100000;

    WMPUInt<kSize1> lhs;
    WMPUInt<kSize2> rhs;
    WMPUInt<kSize1 + kSize2> result, expectedResult;
    const std::size_t tempBufferSize1 = lhs.template fullMultiplyBufferSize<kSize2>();
    const std::size_t tempBufferSize2 = rhs.template fullMultiplyBufferSize<kSize1>();
    const std::size_t tempBufferSize =
        tempBufferSize1 > tempBufferSize2 ? tempBufferSize1 : tempBufferSize2;
    std::uint64_t tempBuffer[tempBufferSize];
    std::uint32_t lhsArray[kSize1*2], rhsArray[kSize2*2];
    std::uint32_t resultArray[kSize1*2+kSize2*2], tempBuffer2[kSize2*2+1];

    for(std::size_t iteration = 0; iteration < kIterations; ++iteration)
    {
        for(std::size_t i = 0; i < kSize1*2; ++i) lhsArray[i] = rng();
        for(std::size_t i = 0; i < kSize2*2; ++i) rhsArray[i] = rng();

        multiply(lhsArray, kSize1*2, rhsArray, kSize2*2, resultArray, tempBuffer2);

        initWMPUIntWithUint32Array(lhs, lhsArray);
        initWMPUIntWithUint32Array(rhs, rhsArray);
        initWMPUIntWithUint32Array(expectedResult, resultArray);

        lhs.fullMultiply(rhs, result, tempBuffer);

        if(result != expectedResult)
            return DPRINT("Error: fullMultiply of\n", lhs, " and\n", rhs,
                          "\nresulted in\n", result, "\ninstead of\n", expectedResult, "\n");

        result.assign(0);
        rhs.fullMultiply(lhs, result, tempBuffer);

        if(result != expectedResult)
            return DPRINT("Error: fullMultiply of\n", rhs, " and\n", lhs,
                          "\nresulted in\n", result, "\ninstead of\n", expectedResult, "\n");

        result.assign(0);
        WMPIntImplementations::doFullLongMultiplication
            (lhs.data(), kSize1, rhs.data(), kSize2, result.data(), tempBuffer);

        if(result != expectedResult)
            return DPRINT("Error: doFullLongMultiplication of\n", rhs, " and\n", lhs,
                          "\nresulted in\n", result, "\ninstead of\n", expectedResult, "\n");

        if constexpr(kSize1 > 1 && kSize1 == kSize2)
        {
            std::uint64_t tempBuffer2[(kSize1+2)*4];
            result.assign(0);
            WMPIntImplementations::doFullKaratsubaMultiplication
                (lhs.data(), kSize1, rhs.data(), kSize2, result.data(), tempBuffer2);

            if(result != expectedResult)
                return DPRINT("Error: doFullKaratsubaMultiplication of\n", rhs, " and\n", lhs,
                              "\nresulted in\n", result, "\ninstead of\n", expectedResult, "\n");
        }
    }

    return true;
}

bool testFullMultiplication()
{
    std::cout << "Testing full multiplication" << std::endl;
    if(!testTheTest()) DRET;

    std::mt19937 rng(1234);
    if(!testFullMultiplication<1, 1>(rng)) DRET;
    if(!testFullMultiplication<2, 1>(rng)) DRET;
    if(!testFullMultiplication<3, 1>(rng)) DRET;
    if(!testFullMultiplication<4, 1>(rng)) DRET;
    if(!testFullMultiplication<5, 1>(rng)) DRET;
    if(!testFullMultiplication<32, 1>(rng)) DRET;
    if(!testFullMultiplication<2, 2>(rng)) DRET;
    if(!testFullMultiplication<3, 2>(rng)) DRET;
    if(!testFullMultiplication<4, 2>(rng)) DRET;
    if(!testFullMultiplication<5, 2>(rng)) DRET;
    if(!testFullMultiplication<32, 2>(rng)) DRET;
    if(!testFullMultiplication<3, 3>(rng)) DRET;
    if(!testFullMultiplication<4, 3>(rng)) DRET;
    if(!testFullMultiplication<5, 3>(rng)) DRET;
    if(!testFullMultiplication<32, 3>(rng)) DRET;
    if(!testFullMultiplication<4, 4>(rng)) DRET;
    if(!testFullMultiplication<5, 4>(rng)) DRET;
    if(!testFullMultiplication<32, 4>(rng)) DRET;
    if(!testFullMultiplication<5, 5>(rng)) DRET;
    if(!testFullMultiplication<6, 5>(rng)) DRET;
    if(!testFullMultiplication<32, 5>(rng)) DRET;
    if(!testFullMultiplication<6, 6>(rng)) DRET;
    if(!testFullMultiplication<7, 6>(rng)) DRET;
    if(!testFullMultiplication<32, 6>(rng)) DRET;
    if(!testFullMultiplication<32, 32>(rng)) DRET;
    return true;
}
