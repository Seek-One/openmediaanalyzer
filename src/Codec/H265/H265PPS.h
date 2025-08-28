#ifndef TOOLKIT_CODEC_UTILS_H265PPS_H_
#define TOOLKIT_CODEC_UTILS_H265PPS_H_

#include <array>
#include <unordered_map>

#include "H265ScalingList.h"
#include "H265NALUnit.h"

struct H265PPSRangeExtension {
	H265PPSRangeExtension();

	uint8_t log2_max_transform_skip_block_size_minus2;
	uint8_t cross_component_prediction_enabled_flag;
	uint8_t chroma_qp_offset_list_enabled_flag;
	uint8_t diff_cu_chroma_qp_offset_depth;
	uint8_t chroma_qp_offset_list_len_minus1;
	std::vector<int8_t> cb_qp_offset_list;
	std::vector<int8_t> cr_qp_offset_list;
	uint8_t log2_sao_offset_scale_luma;
	uint8_t log2_sao_offset_scale_chroma;

	void dump(H26XDumpObject& dumpObject) const;
};

struct H265PPSColourMappingOctants {
	H265PPSColourMappingOctants();

	uint8_t split_octant_flag;
	std::vector<std::vector<std::vector<H265PPSColourMappingOctants>>> sub_color_mapping_octants;
	std::array<uint8_t, 4> coded_res_flag;
	std::array<std::array<uint32_t, 3>, 4> res_coeff_q;
	std::array<std::array<uint32_t, 3>, 4> res_coeff_r;
	std::array<std::array<uint8_t, 3>, 4> res_coeff_s;
};

struct H265PPSColourMappingTable {
	H265PPSColourMappingTable();

	uint8_t num_cm_ref_layers_minus1;
	std::array<uint8_t, 62> cm_ref_layer_id;
	uint8_t cm_octant_depth;
	uint8_t cm_y_part_num_log2;
	uint8_t luma_bit_depth_cm_input_minus8;
	uint8_t chroma_bit_depth_cm_input_minus8;
	uint8_t luma_bit_depth_cm_output_minus8;
	uint8_t chroma_bit_depth_cm_output_minus8;
	uint8_t cm_res_quant_bits;
	uint8_t cm_delta_flc_bits_minus1;
	int16_t cm_adapt_threshhold_u_delta;
	int16_t cm_adapt_threshhold_v_delta;
	H265PPSColourMappingOctants colour_mapping_octants;

	// derived variables
	uint8_t OctantNumC;
	uint8_t OctantNumY;
	uint8_t PartNumY;
	uint8_t BitDepthCmInputY;
	uint8_t BitDepthCmInputC;
	uint8_t BitDepthCmOutputY;
	uint8_t BitDepthCmOutputC;
	uint16_t CMResLSBits;
	uint32_t CMThreshU;
	uint32_t CMThreshV;
};

struct H265PPSMultilayerExtension {
	H265PPSMultilayerExtension();

	uint8_t poc_reset_info_present_flag;
	uint8_t pps_infer_scaling_list_flag;
	uint8_t pps_scaling_list_ref_layer_id;
	uint8_t num_ref_loc_offsets;
	std::vector<uint8_t> ref_loc_offset_layer_id;
	std::vector<uint8_t> scaled_ref_layer_offset_present_flag;
	std::array<int16_t, 64> scaled_ref_layer_left_offset;
	std::array<int16_t, 64> scaled_ref_layer_top_offset;
	std::array<int16_t, 64> scaled_ref_layer_right_offset;
	std::array<int16_t, 64> scaled_ref_layer_bottom_offset;
	std::vector<uint8_t> ref_region_offset_present_flag;
	std::array<int16_t, 64> ref_region_left_offset;
	std::array<int16_t, 64> ref_region_top_offset;
	std::array<int16_t, 64> ref_region_right_offset;
	std::array<int16_t, 64> ref_region_bottom_offset;
	std::vector<uint8_t> resample_phase_set_present_flag;
	std::array<uint8_t, 64> phase_hor_luma;
	std::array<uint8_t, 64> phase_ver_luma;
	std::array<uint8_t, 64> phase_hor_chroma_plus8;
	std::array<uint8_t, 64> phase_ver_chroma_plus8;
	uint8_t colour_mapping_enabled_flag;
	H265PPSColourMappingTable colour_mapping_table;

	void dump(H26XDumpObject& dumpObject) const;
};

struct H265PPSDeltaLookupTable {
	H265PPSDeltaLookupTable();

	uint16_t num_val_delta_dlt;
	uint16_t max_diff;
	uint16_t min_diff_minus1;
	uint16_t delta_dlt_val0;
	std::vector<uint16_t> delta_val_diff_minus_min;
};

struct H265PPS3DExtension {
	H265PPS3DExtension();
	
	uint8_t dlts_present_flag;
	uint8_t pps_depth_layers_minus1;
	uint8_t pps_bit_depth_for_depth_layers_minus8;
	std::array<uint8_t, 64> dlt_flag;
	std::array<uint8_t, 64> dlt_pred_flag;
	std::array<uint8_t, 64> dlt_val_flags_present_flag;
	std::array<std::vector<uint8_t>, 64> dlt_value_flag;
	std::array<H265PPSDeltaLookupTable, 64> delta_dlt;

	void dump(H26XDumpObject& dumpObject) const;
};

struct H265PPSSCCExtension {
	H265PPSSCCExtension();

	uint8_t pps_curr_pic_ref_enabled_flag;
	uint8_t residual_adaptive_colour_transform_enabled_flag;
	uint8_t pps_slice_act_qp_offsets_present_flag;
	int8_t pps_act_y_qp_offset_plus5;
	int8_t pps_act_cb_qp_offset_plus5;
	int8_t pps_act_cr_qp_offset_plus3;
	
	uint8_t pps_palette_predictor_initializers_present_flag;
	uint32_t pps_num_palette_predictor_initializer;
	uint8_t monochrome_palette_flag;
	uint8_t luma_bit_depth_entry_minus8;
	uint8_t chroma_bit_depth_entry_minus8;
	std::array<std::vector<uint32_t>, 3> pps_palette_predictor_initializer;
	void dump(H26XDumpObject& dumpObject) const;
};

struct H265PPS : public H265NALUnit
{
	H265PPS(H265NALHeader* pNALHeader, uint32_t nal_size, const uint8_t* nal_data);
	~H265PPS();

	uint8_t pps_pic_parameter_set_id;
	uint8_t pps_seq_parameter_set_id;
	uint8_t dependent_slice_segments_enabled_flag;
	uint8_t output_flag_present_flag;
	uint8_t num_extra_slice_header_bits;
	uint8_t sign_data_hiding_enabled_flag;
	uint8_t cabac_init_present_flag;
	uint32_t num_ref_idx_l0_default_active_minus1;
	uint32_t num_ref_idx_l1_default_active_minus1;
	int32_t init_qp_minus26;
	uint8_t constrained_intra_pred_flag;
	uint8_t transform_skip_enabled_flag;
	uint8_t cu_qp_delta_enabled_flag;
	uint32_t diff_cu_qp_delta_depth;
	int32_t pps_cb_qp_offset;
	int32_t pps_cr_qp_offset;
	uint8_t pps_slice_chroma_qp_offsets_present_flag;
	uint8_t weighted_pred_flag;
	uint8_t weighted_bipred_flag;
	uint8_t transquant_bypass_enabled_flag;
	uint8_t tiles_enabled_flag;
	uint8_t entropy_coding_sync_enabled_flag;
	uint32_t num_tile_columns_minus1;
	uint32_t num_tile_rows_minus1;
	uint8_t uniform_spacing_flag;
	std::vector<uint32_t> column_width_minus1;
	std::vector<uint32_t> row_height_minus1;
	uint8_t loop_filter_across_tiles_enabled_flag;
	uint8_t pps_loop_filter_across_slices_enabled_flag;
	uint8_t deblocking_filter_control_present_flag;
	uint8_t deblocking_filter_override_enabled_flag;
	uint8_t pps_deblocking_filter_disabled_flag;
	int32_t pps_beta_offset_div2;
	int32_t pps_tc_offset_div2;
	uint8_t pps_scaling_list_data_present_flag;
	H265ScalingList scaling_list_data;
	uint8_t lists_modification_present_flag;
	int32_t log2_parallel_merge_level_minus2;
	uint8_t slice_segment_header_extension_present_flag;

	uint8_t pps_extension_present_flag;
	uint8_t pps_range_extension_flag;
	uint8_t pps_multilayer_extension_flag;
	uint8_t pps_3d_extension_flag;
	uint8_t pps_scc_extension_flag;

	H265PPSRangeExtension pps_range_extension;
	H265PPSMultilayerExtension pps_multilayer_extension;
	H265PPS3DExtension pps_3d_extension;
	H265PPSSCCExtension pps_scc_extension;

	uint8_t TwoVersionsOfCurrDecPicFlag;

	static inline std::unordered_map<uint8_t, H265PPS*> PPSMap;

	void dump(H26XDumpObject& dumpObject) const override;
	void validate() override;
};


#endif // TOOLKIT_CODEC_UTILS_H265PPS_H_
