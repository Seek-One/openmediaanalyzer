#include "H265VPS.h"

H265SubLayerHrdParameters::H265SubLayerHrdParameters()
{

}

H265HrdParameters::H265HrdParameters()
{
	nal_hrd_parameters_present_flag = 0;
	vcl_hrd_parameters_present_flag = 0;
	sub_pic_hrd_params_present_flag = 0;
	tick_divisor_minus2 = 0;
	du_cpb_removal_delay_increment_length_minus1 = 0;
	sub_pic_cpb_params_in_pic_timing_sei_flag = 0;
	dpb_output_delay_du_length_minus1 = 0;
	bit_rate_scale = 0;
	cpb_size_scale = 0;
	cpb_size_du_scale = 0;
	initial_cpb_removal_delay_length_minus1 = 0;
	au_cpb_removal_delay_length_minus1 = 0;
	dpb_output_delay_length_minus1 = 0;
}

H265VPS::H265VPS()
{
	vps_video_parameter_set_id = 0;
	vps_base_layer_internal_flag = 0;
	vps_base_layer_available_flag = 0;
	vps_max_layers_minus1 = 0;
	vps_max_sub_layers_minus1 = 0;
	vps_temporal_id_nesting_flag = 0;
	vps_sub_layer_ordering_info_present_flag = 0;
	vps_max_layer_id = 0;
	vps_num_layer_sets_minus1 = 0;
	vps_timing_info_present_flag = 0;
	vps_num_units_in_tick = 0;
	vps_time_scale = 0;
	vps_poc_proportional_to_timing_flag = 0;
	vps_num_ticks_poc_diff_one_minus1 = 0;
	vps_num_hrd_parameters = 0;
	vps_extension_flag = 0;
}
