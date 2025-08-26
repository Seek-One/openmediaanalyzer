#ifndef TOOLKIT_CODEC_UTILS_H265_SCALING_LIST_H_
#define TOOLKIT_CODEC_UTILS_H265_SCALING_LIST_H_

#include "../H26X/H26XDumpObject.h"

#include "H265Types.h"

struct H265ScalingList {
	H265ScalingList();

	std::vector<UInt8Vector> scaling_list_pred_mode_flag;
	std::vector<UInt32Vector> scaling_list_pred_matrix_id_delta;
	std::vector<UInt32Vector> scaling_list_dc_coef_minus8;
	// scaling_list_dc_coef_minus8[0] -> 16x16
	// scaling_list_dc_coef_minus8[1] -> 32x32
	std::vector<UInt32Matrix> scaling_list_delta_coef;
	std::vector<Int32Matrix> ScalingList;
	// ScalingList[0] -> 4x4
	// ScalingList[1] -> 8x8
	// ScalingList[2] -> 16x16
	// ScalingList[3] -> 32x32

	void dump(H26XDumpObject& dumpObject) const;
};

#endif // TOOLKIT_CODEC_UTILS_H265_SCALING_LIST_H_
