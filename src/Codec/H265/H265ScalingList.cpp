#include <string>
#include <sstream>

#include "H265ScalingList.h"

static int32_t g_defaultIntraScalingList[64] = {
	16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 17, 16, 17, 16, 17, 18,
	17, 18, 18, 17, 18, 21, 19, 20, 21, 20, 19, 21, 24, 22, 22, 24,
	24, 22, 22, 24, 25, 25, 27, 30, 27, 25, 25, 29, 31, 35, 35, 31,
	29, 36, 41, 44, 41, 36, 47, 54, 54, 47, 65, 70, 65, 88, 88, 115
};

static int32_t g_defaultInterScalingList[64] = {
	16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 17, 17, 17, 17, 17, 18,
	18, 18, 18, 18, 18, 20, 20, 20, 20, 20, 20, 20, 24, 24, 24, 24,
	24, 24, 24, 24, 25, 25, 25, 25, 25, 25, 25, 28, 28, 28, 28, 28,
	28, 33, 33, 33, 33, 33, 41, 41, 41, 41, 54, 54, 54, 71, 71, 91
};

H265ScalingList::H265ScalingList()
{
	ScalingList.resize(4);
	for (int k = 0; k < 4; ++k) {
		ScalingList[k].resize(6);
		for (int i = 0; i < 6; ++i) {
			ScalingList[k][i].resize((k == 0 ? 16 : 64));
		}
	}

	// Initialize 4x4 scaling lists (cf 7.4.5 Tab 7-5)
	for (int i = 0; i < 6; ++i) {
		for (int j = 0; j < 16; ++j) {
			ScalingList[0][i][j] = 16;
		}
	}

	// Other scaling lists (cf 7.4.5 Tab 7-6)
	for (int k = 1; k < 4; ++k) {
		for (int i = 0; i < 3; ++i) {
			for (int j = 0; j < 64; ++j) {
				ScalingList[k][i][j] = g_defaultIntraScalingList[j];
				ScalingList[k][i + 3][j] = g_defaultInterScalingList[j];
			}
		}
	}

	// scaling_list_dc_coef_minus8
	scaling_list_dc_coef_minus8.resize(2);
	for (int i = 0; i < 2; ++i) {
		scaling_list_dc_coef_minus8[i].resize(6);
		for (int j = 0; j < 6; ++j) {
			scaling_list_dc_coef_minus8[i][j] = 8;
		}
	}
}

std::vector<std::string> H265ScalingList::dump_fields(){
	std::vector<std::string> fields;
	for(int sizeId = 0;sizeId < 4;++sizeId){
		for(int matrixId = 0;matrixId < 6;matrixId += (sizeId == 3 ? 3 : 1)){
			fields.push_back((std::ostringstream() << "scaling_list_pred_mode_flag[" << sizeId << "][" << matrixId << "]:" << (int)scaling_list_pred_mode_flag[sizeId][matrixId]).str());
			if(!scaling_list_pred_mode_flag[sizeId][matrixId]){
				fields.push_back((std::ostringstream() << "  scaling_list_pred_matrix_id_delta[" << sizeId << "][" << matrixId << "]:" << scaling_list_pred_matrix_id_delta[sizeId][matrixId]).str());
			} else {
				int coefNum = std::min(64, (1 << (4 + (sizeId << 1))));
				if(sizeId > 1){
					fields.push_back((std::ostringstream() << "  scaling_list_dc_coef_minus8[" << sizeId << "][" << matrixId << "]:" << scaling_list_dc_coef_minus8[sizeId][matrixId]).str());
				}
				for(int i = 0;i < coefNum;++i){
					fields.push_back((std::ostringstream() << "    scaling_list_dc_coef_minus8[" << i << "][" << sizeId << "][" << matrixId << "]:" << scaling_list_delta_coef[i][sizeId][matrixId]).str());
				}
			}
		}
	}
	return fields;
}
