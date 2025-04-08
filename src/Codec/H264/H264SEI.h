#ifndef TOOLKIT_CODEC_UTILS_H264SEI_H_
#define TOOLKIT_CODEC_UTILS_H264SEI_H_

#include <cstdint>
#include <string>
#include <vector>

#include "H264NAL.h"

#define SEI_BUFFERING_PERIOD 0
#define SEI_PIC_TIMING 1
#define SEI_FILLER_PAYLOAD 3
#define SEI_USER_DATA_UNREGISTERED 5
#define SEI_RECOVERY_POINT 6
#define SEI_FULL_FRAME_FREEZE 13
#define SEI_MVCD_VIEW_SCALABILITY_INFO 49

struct H264SEIMessage {
	virtual ~H264SEIMessage() = default;
	int payloadType;
	H264SEIMessage();
};

struct H264SEI : public H264NAL {
	H264SEI();
	H264SEI(uint8_t forbidden_zero_bit, uint8_t nal_ref_idc, uint32_t nal_size, uint8_t* nal_data);
	// explicit H264SEI(H264SEI* pH264SEI);
	~H264SEI();

    std::vector<H264SEIMessage*> messages;
	
	std::vector<std::string> dump_fields() override;
};


struct H264SEIBufferingPeriod : public H264SEIMessage {
	~H264SEIBufferingPeriod() = default;
	uint8_t seq_parameter_set_id;
	uint32_t nal_initial_cpb_removal_delay[32];
	uint32_t nal_initial_cpb_removal_delay_offset[32];
	uint32_t vcl_initial_cpb_removal_delay[32];
	uint32_t vcl_initial_cpb_removal_delay_offset[32];
    std::vector<std::string> dump_fields();
};

struct H264SEIPicTiming : public H264SEIMessage {
	~H264SEIPicTiming() = default;
	uint32_t cpb_removal_delay;
	uint32_t dpb_output_delay;
	uint8_t pic_struct;
	uint8_t clock_timestamp_flag[3];
	uint8_t ct_type[3];
	uint8_t nuit_field_based_flag[3];
	uint8_t counting_type[3];
	uint8_t full_timestamp_flag[3];
	uint8_t discontinuity_flag[3];
	uint8_t cnt_dropped_flag[3];
	uint8_t n_frames[3];
	uint8_t seconds_value[3];
	uint8_t minutes_value[3];
	uint8_t hours_value[3];
	uint8_t seconds_flag[3];
	uint8_t minutes_flag[3];
	uint8_t hours_flag[3];
	uint32_t time_offset[3];

	uint8_t seq_parameter_set_id;
    std::vector<std::string> dump_fields();
};

struct H264SEIPanScanRect : public H264SEIMessage {

};

struct H264SEIFillerPayload : public H264SEIMessage {
	std::vector<std::string> dump_fields();
};

struct H264SEIUserDataRegisteredItuTT35 : public H264SEIMessage {

};

struct H264SEIUserDataUnregistered : public H264SEIMessage {
	~H264SEIUserDataUnregistered();
	__uint128_t uuid_iso_iec_11578;
	std::vector<uint8_t> user_data_payload_byte;
	std::vector<std::string> dump_fields();
};

struct H264SEIRecoveryPoint : public H264SEIMessage {
	~H264SEIRecoveryPoint() = default;
	uint32_t recovery_frame_cnt;
	uint8_t exact_match_flag;
	uint8_t broken_link_flag;
	uint8_t changing_slice_group_idc;
	std::vector<std::string> dump_fields();
};

struct H264SEIDecRefPicMarkingRepetition : public H264SEIMessage {

};

struct H264SEISparePic : public H264SEIMessage {

};

struct H264SEISceneInfo : public H264SEIMessage {

};

struct H264SEISubSedInfo : public H264SEIMessage {

};

struct H264SEISubSeqLayerCharacteristics : public H264SEIMessage {

};

struct H264SEISeqCharacteristics : public H264SEIMessage {

};

struct H264SEIFullFrameFreeze : public H264SEIMessage {
	~H264SEIFullFrameFreeze() = default;
	uint16_t full_frame_freeze_repetition_period;
	std::vector<std::string> dump_fields();

};

struct H264SEIFullFrameFreezeRelease : public H264SEIMessage {

};

struct H264SEIFullFrameSnapshot : public H264SEIMessage {

};

struct H264SEIProgressiveRefinementSegmentStart : public H264SEIMessage {

};

struct H264SEIProgressiveRefinementSegmentEnd : public H264SEIMessage {

};

struct H264SEIMotionConstrainedSliceGroupSet : public H264SEIMessage {

};

struct H264SEIFilmGrainCharacteristics : public H264SEIMessage {

};

struct H264SEIDeblockingFilterDisplayPreference : public H264SEIMessage {

};

struct H264SEIStereoVideoInfo : public H264SEIMessage {

};

struct H264SEIPostFilterHint : public H264SEIMessage {

};

struct H264SEIToneMappingInfo : public H264SEIMessage {

};

struct H264SEIScalabilityInfo : public H264SEIMessage {

};

struct H264SEISubPicScalableLayer : public H264SEIMessage {

};

struct H264SEINonRequiredLayerRep : public H264SEIMessage {

};

struct H264SEIPriorityLayerInfo : public H264SEIMessage {

};

struct H264SEILayersNotPresent : public H264SEIMessage {

};

struct H264SEILayerDependencyChange : public H264SEIMessage {

};

struct H264SEIScalableNesting : public H264SEIMessage {

};

struct H264SEIBaseLayerTemporalHrd : public H264SEIMessage {

};

struct H264SEIQualityLayerIntegrityCheck : public H264SEIMessage {

};

struct H264SEIRedundantPicProperty : public H264SEIMessage {

};

struct H264SEITl0DepRepIndex : public H264SEIMessage {

};

struct H264SEITlSwitchingPoint : public H264SEIMessage {

};

struct H264SEIParallelDecodingInfo : public H264SEIMessage {

};

struct H264SEIMvcScalableNesting : public H264SEIMessage {

};

struct H264SEIViewScalabilityInfo : public H264SEIMessage {

};

struct H264SEIMultiviewSceneInfo : public H264SEIMessage {

};

struct H264SEIMultiviewAcquisitionInfo : public H264SEIMessage {

};

struct H264SEINonRequiredViewComponent : public H264SEIMessage {

};

struct H264SEIViewDependencyChange : public H264SEIMessage {

};

struct H264SEIOperationPointsNotPresent : public H264SEIMessage {

};

struct H264SEIBaseViewTemporalHrd : public H264SEIMessage {

};

struct H264SEIFramePackingArrangement : public H264SEIMessage {

};

struct H264SEIMultiviewViewPosition : public H264SEIMessage {

};

struct H264SEIDisplayOrientation : public H264SEIMessage {

};

struct H264SEIMvcdScalableNesting : public H264SEIMessage {

};

struct H264SEIMvcdViewScalabilityInfo : public H264SEIMessage {
	H264SEIMvcdViewScalabilityInfo();
	struct movi { 
		movi();
		uint8_t view_info_depth_view_present_flag;
		uint8_t mvcd_depth_view_flag;
		uint8_t view_info_texture_view_present_flag;
		uint8_t mvcd_texture_view_flag;
	};

	uint16_t num_operation_points_minus1;
	uint16_t operation_point_id[1024];
	uint8_t priority_id[1024];
	uint8_t temporal_id[1024];
	uint16_t num_target_output_views_minus1[1024];
	uint16_t view_id[1024][1024];
	movi view_movi[1024][1024];
	uint8_t profile_level_info_present_flag[1024];
	uint8_t bitrate_info_present_flag[1024];
	uint8_t frm_rate_info_present_flag[1024];
	uint8_t view_dependency_info_present_flag[1024];
	uint8_t  parameters_sets_info_present_flag[1024];
	uint8_t bitstream_restriction_info_present_flag[1024];
	uint32_t op_profile_level_idc[1024];
	uint16_t avg_bitrate[1024];
	uint16_t max_bitrate[1024];
	uint16_t max_bitrate_calc_window[1024];
	uint8_t constant_frm_rate_idc[1024];
	uint16_t avg_frm_rate[1024];
	uint8_t num_directly_dependant_views[1024];
	uint16_t directly_dependant_view_id[1024][16];
	movi directely_dependant_view_movi[1024][16];
	uint16_t view_dependency_info_src_op_id[1024];
	uint8_t num_seq_parameter_sets[1024];
	uint8_t seq_parameter_set_id_delta[1024][32];
	uint8_t num_subset_seq_parameter_set_minus1[1024];
	uint8_t subset_seq_parameter_set_id_delta[1024][32];
	uint8_t num_pic_parameter_set_minus1[1024];
	uint8_t pic_parameter_set_id_delta[1024][256];
	uint16_t parameter_sets_info_src_op_id[1024];
	uint8_t motion_vector_over_pic_boundaries_flag[1024];
	uint8_t max_bytes_per_pic_denom[1024];
	uint8_t max_bits_per_mb_denom[1024];
	uint8_t log2_max_mv_length_horizontal[1024];
	uint8_t log2_max_mv_length_vertical[1024];
	uint8_t num_reorder_frames[1024];
	uint8_t max_dec_frame_buffering[1024];
	std::vector<std::string> dump_fields();
};

struct H264SEIDepthRepresentationInfo : public H264SEIMessage {

};

struct H264SEIThreeDimensionalReferenceDisplaysInfo : public H264SEIMessage {

};

struct H264SEIDepthTiming : public H264SEIMessage {

};

struct H264SEIDepthSamplingInfo : public H264SEIMessage {

};

struct H264SEIConstraintedDepthParameterSetIdentifier : public H264SEIMessage {

};

struct H264SEIGreenMetadata : public H264SEIMessage {

};

struct H264SEIMasteringDisplayColourVolume : public H264SEIMessage {

};

struct H264SEIColourRemappingInfo : public H264SEIMessage {

};

struct H264SEIContentLightLevelInfo : public H264SEIMessage {

};

struct H264SEIAlternativeTransferCharacteristics : public H264SEIMessage {

};

struct H264SEIAmbientViewingEnvironment : public H264SEIMessage {

};

struct H264SEIContentColourVolume : public H264SEIMessage {

};

struct H264SEIEquirectangularProjection : public H264SEIMessage {

};

struct H264SEICubemapProjection : public H264SEIMessage {

};

struct H264SEISphereRotation : public H264SEIMessage {

};

struct H264SEIRegionwisePacking : public H264SEIMessage {

};

struct H264SEIOmniViewport : public H264SEIMessage {

};

struct H264SEIAlternativeDepthInfo : public H264SEIMessage {

};

struct H264SEISeiManifest : public H264SEIMessage {

};

struct H264SEISeiPrefixIndication : public H264SEIMessage {

};

struct H264SEIAnnotatedRegions : public H264SEIMessage {

};

struct H264SEINnPostFilterCharacteristics : public H264SEIMessage {

};

struct H264SEINnPostFilterActivation : public H264SEIMessage {

};

struct H264SEIPhaseIndication : public H264SEIMessage {

};

struct H264SEIShutterIntervalInfo : public H264SEIMessage {

};

struct H264SEIReservedSeiMessage : public H264SEIMessage {

};

#endif // TOOLKIT_CODEC_UTILS_H264SEI_H_
