#ifndef TOOLKIT_CODEC_UTILS_H265ACCESS_UNIT_H_
#define TOOLKIT_CODEC_UTILS_H265ACCESS_UNIT_H_

#include <cstdint>
#include <vector>
#include <memory>
#include <optional>

struct H265NAL;
struct H265Slice;

struct H265AccessUnit {
    enum ReferenceStatus {
        ReferenceStatus_Unused,
        ReferenceStatus_ShortTerm,
        ReferenceStatus_LongTerm
    };

    H265AccessUnit();
    ~H265AccessUnit();

    H265Slice* slice() const;
    std::vector<H265Slice*> slices() const;
    bool empty() const;
    std::vector<H265NAL*> getNALUnits() const;
    void addNALUnit(std::unique_ptr<H265NAL> NALUnit);
    uint32_t count() const;
    uint64_t size() const;
    std::optional<uint16_t> frameNumber() const;
    bool isIRAP() const;
    bool isIDR() const;
    bool isRASL() const;
    bool isRADL() const;
    bool isSLNR() const;
    void validate();
    bool isValid() const;
    bool hasMajorErrors() const;
    bool hasMinorErrors() const;

    std::vector<std::unique_ptr<H265NAL>> NALUnits;
    int32_t PicOrderCntVal;
    uint32_t PicOrderCntMsb;
    bool POCDecoded;
    std::vector<std::string> minorErrors;
    std::vector<std::string> majorErrors;
    ReferenceStatus status;

    // Variables to handle the decoding process 8
	uint8_t TargetDecLayerIdList; // 8.1.2
	uint8_t HighestTid; // 8.1.2
	bool SubPicHrdFlag; // 8.1.2
	std::vector<int32_t> PocStCurrBefore; // 8.3.2
	std::vector<int32_t> PocStCurrAfter; // 8.3.2
	std::vector<int32_t> PocStFoll; // 8.3.2
	std::vector<int32_t> PocLtCurr; // 8.3.2
	std::vector<int32_t> PocLtFoll; // 8.3.2
	std::vector<uint8_t> CurrDeltaPocMsbPresentFlag; // 8.3.2
	std::vector<uint8_t> FollDeltaPocMsbPresentFlag; // 8.3.2
    bool RPSDecoded;

    std::vector<H265AccessUnit*> RefPicSetStCurrBefore;
    std::vector<H265AccessUnit*> RefPicSetStCurrAfter;
    std::vector<H265AccessUnit*> RefPicSetStFoll;
    std::vector<H265AccessUnit*> RefPicSetLtCurr;
    std::vector<H265AccessUnit*> RefPicSetLtFoll;
};

#endif // TOOLKIT_CODEC_UTILS_H265ACCESS_UNIT_H_
