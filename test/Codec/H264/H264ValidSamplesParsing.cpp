#include "Codec/H264/H264AUD.h"
#include "Codec/H264/H264SEI.h"

#include "H264ValidSamplesParsing.h"

const static char g_start_code[4] = {0x00, 0x00, 0x00, 0x01};

H264ValidSamplesParsing::H264ValidSamplesParsing(const char* szDirTestFile)
{
	m_szDirTestFile = szDirTestFile;
}

void H264ValidSamplesParsing::loadStream(const QString& szDirName, H264Stream& stream){
	QDir dirFrame = QDir(QString("%0/stream-samples/%1").arg(m_szDirTestFile, szDirName));

	QStringList listFrame = dirFrame.entryList(QDir::Files, QDir::Name);
	QByteArray bitstream;
	for (int i = 0; i < listFrame.size(); ++i){
		const QString& szFileFrame = listFrame[i];

		QByteArray data = loadFrame(dirFrame, szFileFrame);
		QVERIFY(data.size() > 0);
		bitstream.append(data);
	}

	QVERIFY(stream.parsePacket((uint8_t*)bitstream.data(), bitstream.size()));
}

void H264ValidSamplesParsing::test_h264AxisBitstream()
{
	H264Stream stream;
	loadStream("h264-axis", stream);	
	std::vector<H264AccessUnit*> pAccessUnits = stream.getAccessUnits();
	QVERIFY(pAccessUnits.size() == 64);
	QVERIFY(stream.getGOPs().size() == 2);
	for(H264AccessUnit* pAccessUnit : pAccessUnits){
		H264Slice* pSlice = pAccessUnit->slice();
		QVERIFY(pSlice != nullptr);
		QVERIFY(pAccessUnit->size() == (pSlice->slice_type == H264Slice::SliceType_I ? 3 : 1));
		QVERIFY(pAccessUnit->slices().size() == 1);
	}


	// Data extracted from command: ffmpeg -i bitstream.h264 -c copy -bsf:v trace_headers -f null - 2> /tmp/tmp.txt
	// Command to generate code: cat /tmp/tmp.txt | tr -s " " | sed "s/ [0-9]\+ = \(-\?[0-9]\+\)/==\1/" | cut -d " " -f 5 | sed "s/==/ == /" | sed "s/\(.\+\)/QVERIFY(vps.\1);/"

	// check SPS
	QVERIFY(H264SPS::SPSMap.size() == 1);
	H264SPS* pSPS = H264SPS::SPSMap[0];
	QVERIFY(pSPS != nullptr);
	QVERIFY(pSPS->profile_idc == 66);
	QVERIFY(pSPS->constraint_set0_flag == 0);
	QVERIFY(pSPS->constraint_set1_flag == 0);
	QVERIFY(pSPS->constraint_set2_flag == 0);
	QVERIFY(pSPS->constraint_set3_flag == 0);
	QVERIFY(pSPS->constraint_set4_flag == 0);
	QVERIFY(pSPS->constraint_set5_flag == 0);
	QVERIFY(pSPS->reserved_zero_2bits == 0);
	QVERIFY(pSPS->level_idc == 41);
	QVERIFY(pSPS->seq_parameter_set_id == 0);
	QVERIFY(pSPS->log2_max_frame_num_minus4 == 0);
	QVERIFY(pSPS->pic_order_cnt_type == 0);
	QVERIFY(pSPS->log2_max_pic_order_cnt_lsb_minus4 == 9);
	QVERIFY(pSPS->max_num_ref_frames == 1);
	QVERIFY(pSPS->gaps_in_frame_num_value_allowed_flag == 0);
	QVERIFY(pSPS->pic_width_in_mbs_minus1 == 79);
	QVERIFY(pSPS->pic_height_in_map_units_minus1 == 44);
	QVERIFY(pSPS->frame_mbs_only_flag == 1);
	QVERIFY(pSPS->direct_8x8_inference_flag == 1);
	QVERIFY(pSPS->frame_cropping_flag == 0);
	QVERIFY(pSPS->vui_parameters_present_flag == 1);
	QVERIFY(pSPS->aspect_ratio_info_present_flag == 1);
	QVERIFY(pSPS->aspect_ratio_idc == 1);
	QVERIFY(pSPS->overscan_info_present_flag == 0);
	QVERIFY(pSPS->video_signal_type_present_flag == 1);
	QVERIFY(pSPS->video_format == 5);
	QVERIFY(pSPS->video_full_range_flag == 1);
	QVERIFY(pSPS->colour_description_present_flag == 1);
	QVERIFY(pSPS->colour_primaries == 1);
	QVERIFY(pSPS->transfer_characteristics == 1);
	QVERIFY(pSPS->matrix_coefficients == 1);
	QVERIFY(pSPS->chroma_loc_info_present_flag == 1);
	QVERIFY(pSPS->chroma_sample_loc_type_top_field == 0);
	QVERIFY(pSPS->chroma_sample_loc_type_bottom_field == 0);
	QVERIFY(pSPS->timing_info_present_flag == 0);
	QVERIFY(pSPS->nal_hrd_parameters_present_flag == 0);
	QVERIFY(pSPS->vcl_hrd_parameters_present_flag == 0);
	QVERIFY(pSPS->pic_struct_present_flag == 0);
	QVERIFY(pSPS->bitstream_restriction_flag == 1);
	QVERIFY(pSPS->motion_vectors_over_pic_boundaries_flag == 1);
	QVERIFY(pSPS->max_bytes_per_pic_denom == 0);
	QVERIFY(pSPS->max_bits_per_mb_denom == 0);
	QVERIFY(pSPS->log2_max_mv_length_horizontal == 8);
	QVERIFY(pSPS->log2_max_mv_length_vertical == 7);
	QVERIFY(pSPS->max_num_reorder_frames == 0);
	QVERIFY(pSPS->max_dec_frame_buffering == 1);

	// check PPS
	QVERIFY(H264PPS::PPSMap.size() == 1);
	H264PPS* pPPS = H264PPS::PPSMap[0];
	QVERIFY(pPPS != nullptr);
	QVERIFY(pPPS->pic_parameter_set_id == 0);
	QVERIFY(pPPS->seq_parameter_set_id == 0);
	QVERIFY(pPPS->entropy_coding_mode_flag == 0);
	QVERIFY(pPPS->bottom_field_pic_order_in_frame_present_flag == 0);
	QVERIFY(pPPS->num_slice_groups_minus1 == 0);
	QVERIFY(pPPS->num_ref_idx_l0_active_minus1 == 0);
	QVERIFY(pPPS->num_ref_idx_l1_active_minus1 == 0);
	QVERIFY(pPPS->weighted_pred_flag == 0);
	QVERIFY(pPPS->weighted_bipred_idc == 0);
	QVERIFY(pPPS->pic_init_qp_minus26 == 0);
	QVERIFY(pPPS->pic_init_qs_minus26 == 0);
	QVERIFY(pPPS->chroma_qp_index_offset == 0);
	QVERIFY(pPPS->deblocking_filter_control_present_flag == 1);
	QVERIFY(pPPS->constrained_intra_pred_flag == 0);
	QVERIFY(pPPS->redundant_pic_cnt_present_flag == 0);

	H264Slice* pSlice = pAccessUnits.front()->slice();
	QVERIFY(pSlice != nullptr);
	QVERIFY(pSlice->first_mb_in_slice == 0);
	QVERIFY(pSlice->slice_type == H264Slice::SliceType_I);
	QVERIFY(pSlice->pic_parameter_set_id == 0);
	QVERIFY(pSlice->frame_num == 0);
	QVERIFY(pSlice->idr_pic_id == 0);
	QVERIFY(pSlice->pic_order_cnt_lsb == 0);
	QVERIFY(pSlice->drpm.no_output_of_prior_pics_flag == 0);
	QVERIFY(pSlice->drpm.long_term_reference_flag == 0);
	QVERIFY(pSlice->slice_qp_delta == 2);
	QVERIFY(pSlice->disable_deblocking_filter_idc == 0);
	QVERIFY(pSlice->slice_alpha_c0_offset_div2 == 0);
	QVERIFY(pSlice->slice_beta_offset_div2 == 0);

	pSlice = pAccessUnits[11]->slice();
	QVERIFY(pSlice != nullptr);
	QVERIFY(pSlice->first_mb_in_slice == 0);
	QVERIFY(pSlice->slice_type == H264Slice::SliceType_P);
	QVERIFY(pSlice->pic_parameter_set_id == 0);
	QVERIFY(pSlice->frame_num == 11);
	QVERIFY(pSlice->pic_order_cnt_lsb == 22);
	QVERIFY(pSlice->num_ref_idx_active_override_flag == 0);
	QVERIFY(pSlice->rplr.ref_pic_list_reordering_flag_l0 == 0);
	QVERIFY(pSlice->drpm.adaptive_ref_pic_marking_mode_flag == 0);
	QVERIFY(pSlice->slice_qp_delta == 4);
	QVERIFY(pSlice->disable_deblocking_filter_idc == 0);
	QVERIFY(pSlice->slice_alpha_c0_offset_div2 == 0);
	QVERIFY(pSlice->slice_beta_offset_div2 == 0);
}

void H264ValidSamplesParsing::test_h264IQEyeBitstream(){
	H264Stream stream;
	loadStream("h264-iqeye", stream);
	std::vector<H264AccessUnit*> pAccessUnits = stream.getAccessUnits();
	QVERIFY(pAccessUnits.size() == 61);
	QVERIFY(stream.getGOPs().size() == 2);

	H264AccessUnit* pFirstAccessUnit = pAccessUnits.front();
	QVERIFY(pFirstAccessUnit->size() == 7);
	std::vector<H264NAL*> pFirstAccessUnitNALUnits = pFirstAccessUnit->getNALUnits();
	QVERIFY(pFirstAccessUnitNALUnits[0]->nal_unit_type == H264NAL::UnitType_AUD);
	H264AUD* pAUD = (H264AUD*)pFirstAccessUnitNALUnits[0];
	QVERIFY(pAUD->primary_pic_type == 0);

	QVERIFY(pFirstAccessUnitNALUnits[1]->nal_unit_type == H264NAL::UnitType_SPS);
	QVERIFY(pFirstAccessUnitNALUnits[2]->nal_unit_type == H264NAL::UnitType_PPS);

	for(int i = 3;i < 6;++i) QVERIFY(pFirstAccessUnitNALUnits[i]->nal_unit_type == H264NAL::UnitType_SEI);
	H264SEI* pSEI = (H264SEI*)pFirstAccessUnitNALUnits[4];
	QVERIFY(pSEI->messages.size() == 1);
	QVERIFY(pSEI->messages.front()->payloadType == SEI_PIC_TIMING);
	H264SEIPicTiming* pSEIPicTiming = (H264SEIPicTiming*)pSEI->messages.front();
	QVERIFY(pSEIPicTiming->cpb_removal_delay == 40);
	QVERIFY(pSEIPicTiming->dpb_output_delay == 2);

	QVERIFY(pFirstAccessUnitNALUnits[6]->nal_unit_type == H264NAL::UnitType_IDRFrame);

	H264AccessUnit* pRandomAccessUnit = pAccessUnits[24];
	QVERIFY(pRandomAccessUnit->size() == 3);
	std::vector<H264NAL*> pRandomAccessUnitNALUnits = pRandomAccessUnit->getNALUnits();
	QVERIFY(pRandomAccessUnitNALUnits[0]->nal_unit_type == H264NAL::UnitType_AUD);
	QVERIFY(pRandomAccessUnitNALUnits[1]->nal_unit_type == H264NAL::UnitType_SEI);
	QVERIFY(pRandomAccessUnitNALUnits[2]->nal_unit_type == H264NAL::UnitType_NonIDRFrame);

	H264AccessUnit* pLastAccessUnit = pAccessUnits.back();
	QVERIFY(pLastAccessUnit->size() == 1);
	std::vector<H264NAL*> pLastAccessUnitNALUnits = pRandomAccessUnit->getNALUnits();
	QVERIFY(pLastAccessUnitNALUnits[0]->nal_unit_type == H264NAL::UnitType_AUD);
}

void H264ValidSamplesParsing::test_h264Sony4kBitstream(){
	H264Stream stream;
	loadStream("h264-sony4k", stream);
	std::vector<H264AccessUnit*> pAccessUnits = stream.getAccessUnits();
	QVERIFY(pAccessUnits.size() == 180);
	QVERIFY(stream.getGOPs().size() == 2);
	for(H264AccessUnit* pAccessUnit : pAccessUnits){
		H264Slice* pSlice = pAccessUnit->slice();
		QVERIFY(pSlice != nullptr);
		QVERIFY(pAccessUnit->size() == (pSlice->slice_type == H264Slice::SliceType_I ? 11 : 9));
		QVERIFY(pAccessUnit->slices().size() == 8);
	}

	for(int i = 0;i < pAccessUnits.size();++i){
		std::vector<H264Slice*> pSlices = pAccessUnits[i]->slices();
		QVERIFY(!pSlices.empty());
		if(i%3 == 0) QVERIFY(pSlices.front()->slice_type == H264Slice::SliceType_I || pSlices.front()->slice_type == H264Slice::SliceType_P);
		else QVERIFY(pSlices.front()->slice_type == H264Slice::SliceType_B); // IBBPBBPBBPBB... pattern
		// multi-slice access unit
		uint32_t last_first_mb_in_slice = pSlices.front()->first_mb_in_slice;
		for(H264Slice* pSlice : pSlices){
			QVERIFY(pSlice->frame_num == pSlices.front()->frame_num);
			QVERIFY(pSlice->first_mb_in_slice >= last_first_mb_in_slice);
			last_first_mb_in_slice = pSlice->first_mb_in_slice;
		}
	}
}

QByteArray H264ValidSamplesParsing::loadFrame(const QDir& dirFrame, const QString& szFilename)
{
	QByteArray data;
	QFile fileFrame (dirFrame.filePath(szFilename));

	if (fileFrame.open(QFile::ReadOnly)) {
		data = fileFrame.readAll();
		if(memcmp(data, g_start_code, 4) != 0) data.prepend(g_start_code, 4);
		fileFrame.close();
	}

	return data;
}
