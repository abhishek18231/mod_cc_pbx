//Here we can changes because this is not main branch code. This is changes branch of our code.
#include "cc_pbx.h"   //this is for header
#include <curl/curl.h>
#include <switch_apr.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <esl.h>
#include <cjson/cJSON.h> 

bool file_exists(char *filename)
{
    struct stat buffer;

    if (stat(filename, &buffer) == 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

static int exten_init_callback(void *parg, int argc, char **argv, char **column_names)
{

    ext_detail_t *exten = (ext_detail_t *)parg;
    if (argc < 32)
    {
        switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, " Exten CALLBACK NO DATA %d\n", argc);
        return -1;
    }
    for (int i = 0; i < argc; i++)
        // switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, "exten : %d %s %s\n", i, column_names[i], argv[i]);

        exten->pkg_id = atoi(argv[0]);
    if (!IS_NULL(argv[1]) && strlen(argv[1]))
    {

        exten->cust_id = strdup(argv[1]);
    }
    if (!IS_NULL(argv[2]) && strlen(argv[2]))
    {
        exten->cid_name = strdup(argv[2]);
    }
    exten->dial_timeout = atoi(argv[3]);
    if (!IS_NULL(argv[4]) && strlen(argv[4]))
    {
        exten->external_cid = strdup(argv[4]);
    }
    exten->cid_hdr_type = atoi(argv[5]);
    if (!IS_NULL(argv[6]) && strlen(argv[6]))
    {
        exten->cid_hdr_value = strdup(argv[6]);
    }
    if (!IS_NULL(argv[7]) && strlen(argv[7]))
    {
        exten->codec = strdup(argv[7]);
    }
    exten->is_vm_on = atoi(argv[8]);
    exten->is_dnd = atoi(argv[9]);
    exten->is_outbound_allowed = atoi(argv[10]);
    exten->is_recording_allowed = atoi(argv[11]);
    exten->is_call_frwd = atoi(argv[12]);
    exten->blacklist = atoi(argv[13]);
    exten->is_call_trns = atoi(argv[14]);
    exten->id = atoi(argv[15]);
    exten->is_sd_allowed = atoi(argv[16]);
    if (!IS_NULL(argv[17]) && strlen(argv[17]))
    {
        strcpy(exten->num, argv[17]);
    }
    if (strcmp(argv[18], ""))
    {
        exten->ann_pmt = strdup(argv[18]);
    }
    else
    {
        exten->ann_pmt = "NULL";
    }
    if (!IS_NULL(argv[19]) && strlen(argv[19]))
    {
        exten->mobile = strdup(argv[19]);
    }
    if (!IS_NULL(argv[20]) && strlen(argv[20]))
    {

        exten->is_sms_allowed = strdup(argv[20]);
    }

    exten->is_admin = atoi(argv[21]);
    exten->is_rng_ton = atoi(argv[22]);
    exten->is_fmfm = atoi(argv[23]);
    exten->rng_ton_id = atoi(argv[24]);
    exten->is_mnt_plan_enabled = atoi(argv[25]);
    if (!IS_NULL(argv[26]) && strlen(argv[26]))
    {

    exten->caller_id_name = strdup(argv[26]);
    }
    exten->is_multi_registration = atoi(argv[27]);
    exten->intercom_call = atoi(argv[28]);
    exten->call_waiting = atoi(argv[29]);
    exten->call_persist = atoi(argv[30]);
    if ( (!IS_NULL(argv[31])) && strlen(argv[31]))
    {
       exten->call_back_url = strdup(argv[31]);
    }
    exten->is_init = true;
    return 0;
}

bool get_ext_details(switch_channel_t *channel, ext_detail_t *extn, char *dsn, switch_mutex_t *mutex, const char *num)
{

    if (clr_status(channel))
    {
        char *query;
        switch_channel_set_variable(channel, "ann_pmt", "0");
        query = switch_mprintf("call switch_check_extension_details('%s')", num);
        switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, "Exten query: %s ", query);
        execute_sql_callback(dsn, mutex, query, exten_init_callback, extn);
        switch_safe_free(query);
        
        if (extn->is_init)
        {
            switch_channel_set_variable(channel, "ann_pmt", extn->ann_pmt);
            switch_channel_set_variable(channel, "call_back_url", extn->call_back_url);
            //switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO,"call_back_url %s",extn->call_back_url);
            return true;
        }
    }
    return false;
}

int drule_init_callback(void *parg, int argc, char **argv, char **column_names)
{
    dialout_rule_details_t *drule = (dialout_rule_details_t *)parg;
    if (argc < 7)
    {
        switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, " Dialout_rule CALLBACK ERROR : NO DATA %d\n", argc);
        return -1;
    }

    for (int i = 0; i < argc; i++)
    {
        // switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, "INTERNAL drule data %d %s %s\n", i, column_names[i], argv[i]);
        drule->is_init = true;

        if (!IS_NULL(argv[0]) && strlen(argv[0]))
        {
            drule->strip_digit = strdup(argv[0]);
        }
        if (!IS_NULL(argv[1]) && strlen(argv[1]))
        {
            drule->prepend_digit = strdup(argv[1]);
        }
        drule->black_list = atoi(argv[2]);
        drule->dial_out_as = atoi(argv[3]);
        if (!IS_NULL(argv[4]) && strlen(argv[4]))
        {
            drule->exceptional_rule = strdup(argv[4]);
        }
        drule->id = atoi(argv[5]);
        drule->dialout_group_id = atoi(argv[6]);
    }
    return 0;
}

static int cli_data_init_callback(void *parg, int argc, char **argv, char **column_names)
{
    clr_header_manipulation *cli_data = (clr_header_manipulation *)parg;
    if (argc < 4)
    {
        switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, " clr_header_manipulation CALLBACK  NO DATA %d\n", argc);
        return -1;
    }

    for (int i = 0; i < argc; i++)
        // switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, "INTERNAL cli_data data %d %s %s\n", i, column_names[i], argv[i]);

        cli_data->is_init = true;

    if (!IS_NULL(argv[0]) && strlen(argv[0]))
    {
        cli_data->gw_id = strdup(argv[0]);
    }
    if (!IS_NULL(argv[1]) && strlen(argv[1]))
    {
        cli_data->strip_clr_id = strdup(argv[1]);
    }
    if (!IS_NULL(argv[2]) && strlen(argv[2]))
    {
        cli_data->prepend_clr_id = strdup(argv[2]);
    }
    if (!IS_NULL(argv[3]) && strlen(argv[3]))
    {
        cli_data->clr_id_manipulation = strdup(argv[3]);
    }

    return 0;
}

bool dialoutrule(switch_channel_t *channel, char *dsn, switch_mutex_t *mutex, char *path, char *custom_path, call_details_t *call, const char *dial_num)
{

    if (clr_status(channel))
    {
        char *query;
        char *token = NULL;
        char *rest = NULL;
        const char *original_num = NULL;
        const char *aftr_strp = NULL;
        const char *aftr_prepnd = NULL;
        const char *blist_num = NULL;
        const char *dial_out_as = NULL;
        char *temp_path = NULL;
        const char *cust_id = switch_channel_get_variable(channel, "cust_id");
        char pbx_dialout_rule[20] = {'\0'};
        switch_core_session_t *session = switch_channel_get_session(channel);

        if (IS_NULL(cust_id))
        {
            // switch_log_printf(SWITCH_CHANNEL_SESSION_LOG(session), SWITCH_LOG_ERROR, "Unathroized person %s clr_flag %d cust_id %s  ",switch_channel_get_variable(channel, "network_addr"),clr_status(channel),cust_id);
            switch_channel_hangup(channel, SWITCH_CAUSE_CALL_REJECTED);
            /* query = switch_mprintf("UPDATE `customer` set status =5");
            execute_sql(dsn, query, mutex); */
            return false;
        }

        query = switch_mprintf("select pdr.rule_pattern from  pbx_dialout_rule pdr join customer cust on (pdr.dialout_group_id=cust.dialout_group)and cust.id='%s'", cust_id);
        execute_sql2str(dsn, mutex, query, pbx_dialout_rule, NELEMS(pbx_dialout_rule));
        switch_safe_free(query);

        if (dial_num != NULL)
        {
            query = switch_mprintf("call switch_drule_init_callback_fixed_number('%s','%s')", cust_id, dial_num);
            execute_sql_callback(dsn, mutex, query, drule_init_callback, &call->drule);
            switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, "Dialout Rule Query Fix Number :: %s\n", query);
            switch_safe_free(query);
        }

        // gateway_rule
        if (pbx_dialout_rule[0] == '*' && call->drule.is_init == false)
        {
            query = switch_mprintf("call switch_drule_init_callback_universal('%s')", cust_id);
            switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, "Dialout Rule Query Universal Number :: %s\n", query);
            execute_sql_callback(dsn, mutex, query, drule_init_callback, &call->drule);
            switch_safe_free(query);
        }

        if (call->drule.is_init == false && !call->callee.is_fmfm)
        {
            switch_channel_set_variable(channel, "custom_hangup", "1013");
            switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_WARNING, "%s", switch_channel_get_variable(channel, "custom_hangup"));
            temp_path = switch_mprintf("%s%s", path, "No_dialout_rule.wav");
            switch_ivr_play_file(session, NULL, temp_path, NULL);
            switch_safe_free(temp_path);
            switch_channel_hangup(channel, SWITCH_CAUSE_CALL_REJECTED);
            return false;
        }

        original_num = dial_num;

        if (call->drule.is_init == true)
        {

            if (strncmp(dial_num, call->drule.strip_digit, strlen(call->drule.strip_digit)) == 0)
            {
                aftr_strp = dial_num + strlen(call->drule.strip_digit);
                dial_num = aftr_strp;
            }
            if (call->drule.prepend_digit)
            {
                aftr_prepnd = switch_mprintf("%s%s", call->drule.prepend_digit, dial_num);
                dial_num = aftr_prepnd;
            }
            // switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_WARNING, "dial_num  after manipulation:%s  id %d",dial_num, call->drule.id);

            switch (call->drule.black_list)
            {
            case 1:
                blist_num = original_num;
                break;
            case 2:
                blist_num = aftr_strp;
                break;
            case 3:
                blist_num = aftr_prepnd;
                break;

            default:
                blist_num = dial_num;
                break;
            }
            switch_channel_set_variable(channel, "sip_to_user", blist_num);

            switch (call->drule.dial_out_as)
            {
            case 1:
                dial_out_as = original_num;
                break;
            case 2:
                dial_out_as = aftr_strp;
                break;
            case 3:
                dial_out_as = aftr_prepnd;
                break;

            default:
                dial_out_as = dial_num;
                break;
            }
            switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, "dial_out_as :%s blist_num %s", dial_out_as, blist_num);

            token = strtok_r(call->drule.exceptional_rule, ",", &rest);
            while (token != NULL)
            {

                if (!strncmp(token, blist_num, strlen(token)))
                {
                    switch_log_printf(SWITCH_CHANNEL_SESSION_LOG(session), SWITCH_LOG_ERROR, "Dialed extension/Number is exceptional");
                    sleep(1);
                    temp_path = switch_mprintf("%s%s", path, "blacklisted_according_to_dialout_rule.wav");
                    switch_ivr_play_file(session, NULL, temp_path, NULL);
                    switch_safe_free(temp_path);
                    switch_channel_set_variable(channel, "custom_hangup", "1009");
                    switch_channel_hangup(channel, SWITCH_CAUSE_CALL_REJECTED);
                    return false;
                }
                token = strtok_r(NULL, ",", &rest);
            }

            switch_channel_set_variable(channel, "sip_req_user", dial_num);
            (dial_num[0] != '+') ? dial_num = switch_mprintf("+%s", dial_num) : dial_num;
            call->caller.dout_num = dial_num;
            call->caller.dial_out_as = dial_out_as;

            if (is_black_listed(channel, dsn, mutex, call, blist_num))
            {
                sleep(1);
                temp_path = switch_mprintf("%s%s", path, "call_blacklist.wav");
                switch_ivr_play_file(session, NULL, temp_path, NULL);
                switch_safe_free(temp_path);
                switch_channel_hangup(channel, SWITCH_CAUSE_CALL_REJECTED);
                return false;
            }

            if (is_black_list_outgoing(channel, dsn, mutex, call, blist_num))
            {
                switch_log_printf(SWITCH_CHANNEL_SESSION_LOG(session), SWITCH_LOG_ERROR, "Dialed extension/Number is blacklisted as :: Outgoing Number.\n");
                sleep(1);
                temp_path = switch_mprintf("%s%s", path, "call_blacklist.wav");
                switch_ivr_play_file(session, NULL, temp_path, NULL);
                switch_safe_free(temp_path);
                switch_channel_hangup(channel, SWITCH_CAUSE_CALL_REJECTED);
                return false;
            }

            return true;
        }
    }

    return false;
}

bool is_black_listed(switch_channel_t *channel, char *dsn, switch_mutex_t *mutex, call_details_t *call, const char *blist_num)
{
    // switch_core_session_t *session = switch_channel_get_session(channel);
    char result[20] = {'\0'};

    if (clr_status(channel))
    {
        char *query;
        // const char *ext_id = switch_channel_get_variable(channel, "ext_id");
        const char *caller = switch_channel_get_variable(channel, "sip_from_user");
        const char *cust_id = switch_channel_get_variable(channel, "cust_id");
        char *clr_num;
        // switch_core_session_execute_application(session, "info",NULL);
        if (strlen(caller) > 8)
        {

            clr_num = switch_mprintf("%%%s", caller);
        }
        else if (caller != NULL && blist_num == NULL)
        {
            clr_num = switch_mprintf("%s", caller);
        }
        else
        {
            clr_num = switch_mprintf("%s", blist_num);
        }
        // switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_DEBUG, " Incoming caller_id::%d  ext_id %s cust_id:%s blacklist number: :%s",  call->caller.id,ext_id,cust_id,clr_num);
        query = switch_mprintf("call switch_check_is_black_listed('%s',%d,'%s')", cust_id, call->caller.id, clr_num);
        execute_sql2str(dsn, mutex, query, result, NELEMS(result));
        switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, "Black List Validation Procedure Call:: %s", query);

        switch_safe_free(query);

        if (strlen(result) != 0)
        {
            switch_channel_set_variable(channel, "custom_hangup", "1009");
        }
    }
    return strlen(result) ? true : false;
}

bool is_black_list_outgoing(switch_channel_t *channel, char *dsn, switch_mutex_t *mutex, call_details_t *call, const char *blist_num)
{
    char result[20] = {'\0'};

    if (clr_status(channel))
    {
        char *query;

        const char *cust_id = switch_channel_get_variable(channel, "cust_id");
        const char *callee = switch_channel_get_variable(channel, "transfer_number");
        switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_DEBUG, "Black List Validation as :: Outgoing Number :: cust_id=%s ,blist_num=%s ,callee=%s", cust_id, blist_num, callee);

        query = switch_mprintf("select phone_number from pbx_black_list where status='1' and (blocked_for='2' or blocked_for='3')\
    and customer_id in ('%s','%s') and extension_id in (0,%d)  and phone_number in ('%s','%s')",
                               call->caller.cust_id, cust_id, call->caller.id, callee, blist_num);
        execute_sql2str(dsn, mutex, query, result, NELEMS(result));
        // switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, "  black_list check callee:%s  result : %s  Query : %s :: %d\n",call->callee.num,result, query, IS_NULL(result));
        switch_safe_free(query);
        if (strlen(result) != 0)
        {
            switch_channel_set_variable(channel, "custom_hangup", "1009");
        }
    }
    return strlen(result) ? true : false;
}

void set_recording(switch_channel_t *channel, const char *type, call_details_t *call, char *dsn, switch_mutex_t *mutex, char *recording_path, char *custom_path)
{
    if (clr_status(channel))
    {
        const char *re_type = NULL;
        char *tmp_str = NULL;
        switch_time_exp_t tm;
        char date[50] = "";
        switch_size_t retsize;
        switch_time_t ts;
        const char *callee = switch_channel_get_variable(channel, "sip_req_user");
        const char *caller = switch_channel_get_variable(channel, "sip_from_user");
        const char *voicemail_id = NULL;
        switch_core_session_t *session = switch_channel_get_session(channel);
        if (!strcmp(type, "Call Group"))
        {
            re_type = "cg";
        }
        else if (!strcmp(type, "Call Group Sticky Agent"))
        {
            re_type = "cgsa";
        }
        else if (!strcmp(type, "Call Queue Sticky Agent"))
        {
            re_type = "cqsa";
        }
        else if (!strcmp(type, "Outbound Call"))
        {
            re_type = "outbound";
        }
        else if (!strcmp(type, "Conference Call"))
        {
            re_type = "conf";
        }
        else
        {
            re_type = type;
        }
        ts = switch_time_now();
        switch_time_exp_lt(&tm, ts);
        switch_strftime(date, &retsize, sizeof(date), "%Y-%m-%d-%T", &tm);
        switch_core_session_execute_application(session, "set", "RECORD_STEREO=true");
        switch_core_session_execute_application(session, "set", "media_bug_answer_req=true");
        switch_channel_set_variable(channel, "rec_callee", callee);

        /* switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, "RECORD DETAILS: Type:%s DATE:%s Callee:%s\n Record_path:%s cust_id:%s" , type, date,callee
           ,call->flags.recording_path,switch_channel_get_variable(channel, "cust_id") );*/

        tmp_str = switch_mprintf("%s/%s/recording/%s_clr%s_cle%s_%s.wav", call->flags.recording_path, switch_channel_get_variable(channel, "cust_id"), re_type, caller, callee, date);
        switch_core_session_execute_application(session, "record_session", tmp_str);
        /* tmp_str  =switch_mprintf("%s start en-US ",switch_channel_get_uuid(channel));
        switch_core_session_execute_application(session, "uuid_google_transcribe", tmp_str); */
        // switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, "recording_file : %s", tmp_str);

        switch_channel_set_variable(channel, "recording_file", tmp_str);
        switch_channel_set_variable(channel, "set_recording", "1");
        switch_channel_set_variable(channel, "recording_path", recording_path);
        switch_channel_set_variable(channel, "rec_cust_id", call->caller.cust_id);
        switch_channel_set_variable(channel, "rec_type", type);
        switch_channel_set_variable(channel, "rec_num", call->caller.num);
        switch_channel_set_variable(channel, "rec_date", date);
        voicemail_id = switch_mprintf("%d", call->callee.id);
        switch_channel_set_variable(channel, "vm_id", voicemail_id);
        switch_safe_free(tmp_str);
    }
}

void voicemail(switch_core_session_t *session, char *check, char *auth, const char *num)
{
    char *tmp_str = NULL;
    switch_channel_t *channel = switch_core_session_get_channel(session);
    const char *ann_pmt = switch_channel_get_variable(channel, "ann_pmt");
    const char *call_type = switch_channel_get_variable(channel, "call_type");

    if (call_type != NULL && (strcmp(call_type, "sip_extn")))
    {
        switch_channel_answer(channel);
        switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_WARNING, "call_type %s  DETAILS: %d", call_type, strcmp(call_type, "sip_extn"));
        switch_channel_set_variable(channel, "DIALSTATUS", "SUCCESS");
    }
    switch_channel_set_variable(channel, "call_type", "call_voicemail");
    tmp_str = switch_mprintf("%s %s default %s %s", check, auth, switch_channel_get_variable(channel, "opsip_ip_port"), num);
    // tmp_str = switch_mprintf("%s %s default localhost %s", check, auth, num);
    if (strcmp(ann_pmt, "1"))
        switch_core_session_execute_application(session, "set", "skip_greeting=true");
    switch_core_session_execute_application(session, "voicemail", tmp_str);
    switch_safe_free(tmp_str);
}

bool bridge_call(switch_core_session_t *session, call_details_t *call, const char *dial_num, char *dsn, switch_mutex_t *mutex)
{
    char blnce[20];
    char crdt_lmt[20];
    char billing_typ[20];
    char *tmp_str = NULL;
    char *call_hdr = NULL;
    uint8_t dial_timeout = 60;
    originated_header result;
    switch_channel_t *channel = switch_core_session_get_channel(session);
    char const *call_type = switch_channel_get_variable(channel, "call_type");
    char *caller_id = "0", caller_id_hdr[20] = {'\0'}, origination_caller_number[20] = {'\0'}, caller_id_name[20] = {'\0'}, caller[20] = {'\0'}, *sql = NULL;
    char *call_procedure = NULL;
    char geo_location[5] = {'\0'};
    char str_num[50] = {'\0'};
    char cli_num[50] = "{\0}";
    char **token_strip, **token_prepend, **clr_id_manipulation;
    int num;
    const char *dialstatus;
    const char *opsip_ip_port = switch_channel_get_variable(channel, "opsip_ip_port");
    const char *cust_id = switch_channel_get_variable(channel, "cust_id");
    // char *esl_handle=NULL;
    // const char* uuid = switch_channel_get_variable(channel, "uuid");

    if (clr_status(channel))
    {
        call_procedure = switch_mprintf("CALL switch_get_geo_tracking('%s')", cust_id);
        execute_sql2str(dsn, mutex, call_procedure, geo_location, NELEMS(geo_location));
        switch_channel_set_variable(channel, "geo_location", geo_location);
        // switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, "CALLER CUST_ID:%s  & geo_location:%s var_get_loc:%s dial_num :%s",cust_id,geo_location, switch_channel_get_variable(channel, "geo_location"), dial_num);

        if (call->flags.is_outbound_mnt || call->flags.is_outbound_grp_mnt)
        {
            call->obd.call_hdr_typ = atoi(call->mnt.gw_hdr_typ);
            call->obd.gw_codec = call->mnt.gw_codec;
            call->obd.clr_id_hdr = call->mnt.gw_clr_id_hdr;
            call->obd.clr_id = call->mnt.gw_caller_id;
            call->obd.gw_prepend_cid = call->mnt.gw_prepend_cid;
            call->obd.gw_id = call->mnt.gw_id;
            call->obd.call_plan_id = call->mnt.dlout_cp_id;
            call->obd.dial_prefix = call->mnt.dial_prefix;
            // call->obd.call_plan_rate_id=call->mnt.dlout_cp_id;
            call->obd.strp_clr_id = call->mnt.strp_clr_id;
        }

        // switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, " fmfm dial_tm_out :%s is_outbound  %d",switch_channel_get_variable(channel, "dial_tm_out"),call->flags.is_call_outbound);
        if (call->flags.is_frwd_outbound || call->flags.is_call_outbound)
        {
            // int dial_timeout = (call->flags.is_frwd_outbound == true) ? call->callee.dial_timeout : call->caller.dial_timeout;
            sprintf(blnce, "%f", call->obd.blnce);
            sprintf(crdt_lmt, "%f", call->obd.crdt_lmt);
            sprintf(billing_typ, "%d", call->obd.billing_typ);
            switch_channel_set_variable(channel, "application", "outbound");
            switch_channel_set_variable(channel, "sell_rate", call->obd.sell_rate);
            switch_channel_set_variable(channel, "selling_min_duration", call->obd.min_dur);
            switch_channel_set_variable(channel, "selling_billing_block", call->obd.incr_dur);
            switch_channel_set_variable(channel, "call_plan_id", call->obd.call_plan_id);
            switch_channel_set_variable(channel, "buy_rate", call->obd.buy_rate);
            switch_channel_set_variable(channel, "gateway_group_id", call->obd.gw_id);
            switch_channel_set_variable(channel, "dial_prefix", call->obd.dial_prefix);
            // switch_channel_set_variable(channel,"talking_mnt",call->obd.talktime_mnt);
            switch_channel_set_variable(channel, "talking_blance", blnce);
            switch_channel_set_variable(channel, "credit_blance", crdt_lmt);
            switch_channel_set_variable(channel, "account_type", billing_typ);
            switch_channel_set_variable(channel, "call_plan_rate_id", call->obd.call_plan_rate_id);
            strcpy(origination_caller_number, "+91");

            if (call->obd.call_hdr_typ == 0)
            {
                call_hdr = "rpid";
            }
            else if (call->obd.call_hdr_typ == 1)
            {
                call_hdr = "pid";
            }
            else
            {
                call_hdr = "none";
            }

            dial_num = call->caller.dial_out_as;
            switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, "Dial tring after Manipulation :: %s ", dial_num);

            if (call->did.is_init)
            {
                caller_id = call->did.num;
                if (call->callee.is_init)
                {
                    dial_timeout = call->callee.dial_timeout;
                }
            }
            else if (call->caller.is_init)
            {
                dial_timeout = call->caller.dial_timeout;
                caller_id = call->caller.cid_hdr_value;
            }

            if (!IS_NULL(switch_channel_get_variable(channel, "dial_tm_out")))
            {
                dial_timeout = atoi(switch_channel_get_variable(channel, "dial_tm_out"));
            }

            switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, "Extension CallerID or DID num :: %d", !atoi(caller_id));

            if (!atoi(caller_id))
            {

                sql = switch_mprintf("call switch_did_master(%s)", cust_id);
                execute_sql2str(dsn, mutex, sql, caller_id_hdr, NELEMS(caller_id_hdr));
                caller_id = caller_id_hdr;
                switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, "Master CallerID :: %s ", caller_id);

                if (atoi(caller_id) == 0)
                {
                    caller_id = call->obd.clr_id_hdr;
                }
            }

            // switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_WARNING, " caller_id:%s caller:%s" ,caller_id,switch_channel_get_variable(channel, "sip_from_user"));

            call_procedure = switch_mprintf("select GROUP_CONCAT(cli.gw_id) as gw_id, GROUP_CONCAT(case when cli.strip_clr_id='' then 'na' else  cli.strip_clr_id end)  as strip ,\
    GROUP_CONCAT( case when cli.prepend_clr_id='' then 'na' else  cli.prepend_clr_id end)  as prepend,\
    GROUP_CONCAT(cli.clr_id_manipulation)  as manipulation  from pbx_caller_header_manipulation cli  WHERE cli.gw_id = %s",
                                            call->obd.gw_id);
            execute_sql_callback(dsn, mutex, call_procedure, cli_data_init_callback, &call->cli_data);

            token_strip = split(call->cli_data.strip_clr_id, &num, ",", channel);
            token_prepend = split(call->cli_data.prepend_clr_id, &num, ",", channel);
            clr_id_manipulation = split(call->cli_data.clr_id_manipulation, &num, ",", channel);
            // switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, " prendn :%s",token_prepend[0]);

            call_procedure = switch_mprintf("call switch_vmn_cli_manipulation('%s')", caller_id);
            execute_sql2str(dsn, mutex, call_procedure, cli_num, NELEMS(cli_num));
            caller_id = cli_num;

            if (!IS_NULL(caller_id) && call->cli_data.is_init)
            {
                for (int i = 0; i < atoi(switch_channel_get_variable(channel, "split_index")); i++)
                {
                    if ((*clr_id_manipulation[i] == '2') && strncmp(token_strip[i], caller_id, strlen(token_strip[i])) == 0 && strcmp(token_prepend[i], "na"))
                    {
                        strcpy(str_num, caller_id + strlen(token_strip[i]));
                        caller_id = switch_mprintf("%s%s", token_prepend[i], str_num);
                        break;
                    }
                    else if (*clr_id_manipulation[i] == '3' && strcmp(token_prepend[i], "na"))
                    {
                        if (!strcmp(token_strip[i], "na"))
                        {
                            caller_id = switch_mprintf("%s%s", token_prepend[i], caller_id);
                            break;
                        }
                        else if ((strncmp(token_strip[i], caller_id, strlen(token_strip[i])) == 0) && strcmp(token_prepend[i], "na"))
                        {
                            caller_id = switch_mprintf("%s%s", token_prepend[i], caller_id);
                            break;
                        }
                    }
                }
            }
            else if (caller_id == NULL && !IS_NULL(call_type) && !strcmp(call_type, "call_plugin"))
            {
                caller_id = switch_mprintf("%s", switch_channel_get_variable(channel, "sip_from_user"));
            }

            switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_WARNING, "Final CallerID gateway :: %s , Callee :: %s ,Callee_hdr_value :: %s ,Name :: %s ,Dial_timeout :: %d", caller_id, call->callee.num, call->callee.cid_hdr_value, call->callee.cid_name, dial_timeout);
            switch_core_session_execute_application(session, "set", "sip_copy_custom_headers=false");

            tmp_str = switch_mprintf("{absolute_codec_string='%s',sip_contact_user='demo124689',sip_cid_type=%s,ignore_early_media=false,origination_caller_id_name=%s,outbound_caller_from_user=%s,user_type=Agent\
                  ,origination_caller_id_number=%s,originate_timeout=%d,cust_id='%s',custom_uuid='%s'}sofia/gateway/gw_%s/%s",
                                     call->obd.gw_codec, call_hdr, caller_id, caller_id, caller_id, dial_timeout, cust_id, call->patch.custom_uuid, call->obd.gw_id, dial_num);
        }
        else
        {

            strcpy(caller, switch_channel_get_variable(channel, "ani"));
            switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_DEBUG, "CallerID::%s ,Caller_id_name::%s", caller, call->callee.caller_id_name);

            if (call->did.is_init)
            {
                switch_channel_set_variable(channel, "application", "inbound");

                result = originated_id_manipulation(channel, caller, caller, call);
                strcpy(caller_id_name, result.originated_name);
                strcpy(origination_caller_number, result.originated_name);
                switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_DEBUG, "caller_id_name PSTN:%s", caller_id_name);

                if (call->callee.is_init)
                {
                    dial_timeout = call->callee.dial_timeout;
                }
            }

            else if (switch_channel_get_variable(channel, "set_frwd") != NULL)
            {
                dial_timeout = call->callee.dial_timeout;
                strcpy(origination_caller_number, call->callee.num);
                strcpy(caller_id_name, call->callee.caller_id_name);
                dial_timeout = call->caller.dial_timeout;
                switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, "call->caller.is_init:%d origination_caller_number:%s", call->caller.is_init, call->callee.num);
            }
            else if (call->caller.is_init)
            {
                switch_channel_set_variable(channel, "application", "intercom");
                strcpy(origination_caller_number, caller);
                strcpy(caller_id_name, call->caller.caller_id_name);
                dial_timeout = call->caller.dial_timeout;
            }

            if (switch_channel_get_variable(channel, "dial_tm_out") != NULL)
            {
                dial_timeout = atoi(switch_channel_get_variable(channel, "dial_tm_out"));
            }

            switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, "origination_caller_number:%s caller_id_name:%s dial_timeout:%d dial_num:%s set_frw:%s", origination_caller_number, caller_id_name, dial_timeout, dial_num, switch_channel_get_variable(channel, "set_frwd"));

            if (switch_channel_get_variable(channel, "sip_h_X-calleetype") == NULL)
            {
                switch_channel_set_variable(channel, "sip_h_X-calleetype", "SIP");
                switch_channel_set_variable(channel, "sip_h_X-callertype", "SIP");
                // switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, "SIP sip_h_X-callee--> %s",switch_channel_get_variable(channel,"sip_h_X-callertype"));
            }

            if (switch_channel_get_variable(channel, "sip_h_X-calleetype") != NULL && switch_channel_get_variable(channel, "sip_h_X-callertype") != NULL)
            {
                if (!strcmp(switch_channel_get_variable(channel, "sip_h_X-callertype"), "SIP") && !strcmp(switch_channel_get_variable(channel, "sip_h_X-calleetype"), "SIP"))
                {

                    tmp_str = switch_mprintf("{origination_caller_id_number='%s',media_mix_inbound_outbound_codecs=true,ignore_early_media=true,outbound_caller_from_user='%s',origination_caller_id_name='%s',originate_timeout=%d }user/%s\
    ",
                                             origination_caller_number, origination_caller_number, caller_id_name, dial_timeout, dial_num);
                }

                // tmp_str = switch_mprintf("{origination_caller_id_number='%s',media_mix_inbound_outbound_codecs=true,ignore_early_media=true,outbound_caller_from_user='%s',origination_caller_id_name='%s',originate_timeout=%d  }user/%s",origination_caller_number, origination_caller_number,caller_id_name,dial_timeout,dial_num); }

                else if (!strcmp(switch_channel_get_variable(channel, "sip_h_X-callertype"), "WS") && !strcmp(switch_channel_get_variable(channel, "sip_h_X-calleetype"), "WS"))
                {
                    // switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO,"------- WebRTC-2-WebRTC Call " );
                    tmp_str = switch_mprintf("{media_webrtc=true,media_mix_inbound_outbound_codecs=true,ignore_early_media=true,origination_caller_id_name='%s',originate_timeout=%d}sofia/internal/%s@%s\
    ",
                                             switch_channel_get_variable(channel, "sip_from_user"), dial_timeout, dial_num, opsip_ip_port);
                }
                else if (!strcmp(switch_channel_get_variable(channel, "sip_h_X-callertype"), "SIP") && !strcmp(switch_channel_get_variable(channel, "sip_h_X-calleetype"), "WS"))
                {
                    // switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO,"------- SIP-2-WebRTC Call " );
                    tmp_str = switch_mprintf("{media_webrtc=true,media_mix_inbound_outbound_codecs=true,ignore_early_media=true,origination_caller_id_name='%s',originate_timeout=%d}sofia/internal/%s@%s\
    ",
                                             switch_channel_get_variable(channel, "sip_from_user"), dial_timeout, dial_num, opsip_ip_port);
                }
                else if (!strcmp(switch_channel_get_variable(channel, "sip_h_X-callertype"), "WS") && !strcmp(switch_channel_get_variable(channel, "sip_h_X-calleetype"), "SIP"))
                {
                    // switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO,"------- WebRTC-2-SIP Call  " );

                    tmp_str = switch_mprintf("{media_mix_inbound_outbound_codecs=true,ignore_early_media=true,origination_caller_id_name='%s',originate_timeout=%d}sofia/internal/%s@%s\
    ",
                                             switch_channel_get_variable(channel, "sip_from_user"), dial_timeout, dial_num, opsip_ip_port);
                }
            }
            // switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, "Dial_string  SIP EXTENSION: %s\n", tmp_str);
        }

        switch_core_session_execute_application(session, "bridge", tmp_str);
        switch_safe_free(tmp_str);
        dialstatus = switch_channel_get_variable(channel, "DIALSTATUS");

        if (!(IS_NULL(dialstatus)))
        {
            const char *status[] = {"SUCCESS", "CANCEL"};
            for (size_t i = 0; i < sizeof(status) / sizeof(status[0]); i++)
            {
                call->flags.isCallerHangup = strcmp(dialstatus, status[i]);
                if (!(call->flags.isCallerHangup))
                {
                    break;
                }
            }
        }
        switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_WARNING, "DIALSTATUS ::%s %d", dialstatus, call->flags.isCallerHangup);
    }
    return call->flags.isCallerHangup;
}

static int call_frwd_callback(void *parg, int argc, char **argv, char **column_name)
{

    call_frwd_t *cfrwd = (call_frwd_t *)parg;
    if (argc < 8)
    {
        switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, "Call frwd : callback \n");
        return -1;
    }

    cfrwd[0].type = atoi(argv[0]);
    if (!IS_NULL(argv[1]) && strlen(argv[1]))
    {
        strcpy(cfrwd[0].num, argv[1]);
    }
    cfrwd[1].type = atoi(argv[2]);
    if (!IS_NULL(argv[3]) && strlen(argv[3]))
    {
        strcpy(cfrwd[1].num, argv[3]);
    }
    cfrwd[2].type = atoi(argv[4]);
    if (!IS_NULL(argv[5]) && strlen(argv[5]))
    {
        strcpy(cfrwd[2].num, argv[5]);
    }
    cfrwd[3].type = atoi(argv[6]);
    if (!IS_NULL(argv[7]) && strlen(argv[7]))
    {
        strcpy(cfrwd[3].num, argv[7]);
    }
    // cfrwd->is_init = true;

    return 0;
}

void check_call_frwd(switch_channel_t *channel, char *dsn, switch_mutex_t *mutex, call_details_t *call)
{
    char *query = NULL;
    if (!(call->callee.is_init))
    {
        switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, "Callee not system user. No check for frwd\n");
        return;
    }
    query = switch_mprintf("call switch_check_call_frwd('%s',%d) ", call->callee.cust_id, call->callee.id);
    execute_sql_callback(dsn, mutex, query, call_frwd_callback, &call->frwd);
    switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, "frwding query %s", query);
    switch_safe_free(query);
}

void forward_call(switch_channel_t *channel, char *dsn, switch_mutex_t *mutex, char *path, char *custom_path, call_details_t *call, int i)
{
    if (clr_status(channel))
    {
        char *temp_path;
        switch_core_session_t *session = switch_channel_get_session(channel);
        if (call->callee.is_dnd == true)
        {
            sleep(1);
            temp_path = switch_mprintf("%s%s", path, "ivr-dnd_activated.wav");
            switch_ivr_play_file(session, NULL, temp_path, NULL);
            switch_safe_free(temp_path);
            switch_channel_hangup(channel, SWITCH_CAUSE_CALL_REJECTED);
            return;
        }
        // switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, "%d", call->callee.is_dnd);

        // Handle Blacklisted number
        if ((call->callee.blacklist == true) && (is_black_listed(channel, dsn, mutex, call, NULL)))
        {
            switch_log_printf(SWITCH_CHANNEL_SESSION_LOG(session), SWITCH_LOG_ERROR, "Dialed extension/Number is blacklisted.\n");
            sleep(1);
            temp_path = switch_mprintf("%s%s", path, "call_forwarding.wav");
            switch_ivr_play_file(session, NULL, temp_path, NULL);
            switch_safe_free(temp_path);
            switch_channel_hangup(channel, SWITCH_CAUSE_CALL_REJECTED);
            return;
        }
        if ((call->caller.blacklist == true) && (is_black_list_outgoing(channel, dsn, mutex, call, NULL)))
        {
            switch_log_printf(SWITCH_CHANNEL_SESSION_LOG(session), SWITCH_LOG_ERROR, "Dialed extension/Number is blacklisted.\n");
            sleep(1);
            temp_path = switch_mprintf("%s%s", path, "call_blacklist.wav");
            switch_ivr_play_file(session, NULL, temp_path, NULL);
            switch_safe_free(temp_path);

            switch_channel_hangup(channel, SWITCH_CAUSE_CALL_REJECTED);
            return;
        }

        // switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, "Type: %d  num:%s", call->frwd[i].type ,call->frwd[i].num);
        call->flags.is_frwded = true;
        switch (call->frwd[i].type)
        {
        case 1:
            switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, "forwarding to voicemail: %s\n", call->callee.num);
            if (call->callee.is_vm_on)
            {
                voicemail(session, NULL, NULL, call->callee.num);
                switch_channel_hangup(channel, SWITCH_CAUSE_CALL_REJECTED);
                break;
            }
            break;
        case 2:
            call->flags.is_frwd_outbound = true;
            switch_channel_set_variable(channel, "set_frwd", "1");
            dialoutrule(channel, dsn, mutex, path, custom_path, call, call->frwd[i].num);
            outbound(session, dsn, mutex, path, custom_path, call, call->frwd[i].num);
            (call->obd.gw_id == NULL) ? call->obd.gw_id = call->mnt.gw_id : call->obd.gw_id;

            if (call->obd.gw_id != 0)
            {
                sleep(1);
                temp_path = switch_mprintf("%s%s", path, "call_forwarding.wav");
                switch_ivr_play_file(session, NULL, temp_path, NULL);
                switch_safe_free(temp_path);
                bridge_call(session, call, call->frwd[i].num, dsn, mutex);
                switch_channel_hangup(channel, SWITCH_CAUSE_CALL_REJECTED);
            }
            else /* if (!(call->callee.is_fmfm)  && call->obd.gw_id == NULL ) */
            {
                switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, " NO dialout rates found on forwarding ");
                switch_channel_set_variable(channel, "custom_hangup", "1012");
                temp_path = switch_mprintf("%s%s", path, "dialout_rate_missing.wav");
                switch_ivr_play_file(session, NULL, temp_path, NULL);
                switch_safe_free(temp_path);
                switch_channel_hangup(channel, SWITCH_CAUSE_CALL_REJECTED);
                return;
            }

            switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, "forwarding to outbound: %s  gateway_id:%s ", call->frwd[i].num, call->obd.gw_id);

            break;
        case 3:
            switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, "forwarding to extension: %s\n", call->frwd[i].num);
            switch_channel_set_variable(channel, "set_frwd", "1");
            get_ext_details(channel, &call->callee, dsn, mutex, call->frwd[i].num);
            if (call->callee.is_dnd == true)
            {
                sleep(1);
                temp_path = switch_mprintf("%s%s", path, "ivr-dnd_activated.wav");
                switch_ivr_play_file(session, NULL, temp_path, NULL);
                switch_safe_free(temp_path);
                switch_channel_hangup(channel, SWITCH_CAUSE_CALL_REJECTED);
                return;
            }

            bridge_call(session, call, call->frwd[i].num, dsn, mutex);
            call->callee.is_vm_on = false;
            //  switch_channel_hangup(channel,SWITCH_CAUSE_CALL_REJECTED);
            break;
        default:
            call->flags.is_frwded = false;
            switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, "Forwarding enabled but No setup found\n");
        }
    }
}

static int outbound_callback(void *parg, int argc, char **argv, char **column_names)
{
    outbound_detail_t *obd = (outbound_detail_t *)parg;

    if (argc < 26)
    {
        switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, " Outbound_callback  NO DATA %d\n", argc);
        return -1;
    }

    for (int i = 0; i < argc; i++)
        // switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, "outbound_detail : %d %s %s\n", i, column_names[i], argv[i]);

        obd->is_init = true;
    if (!IS_NULL(argv[0]) && strlen(argv[0]))
    {
        obd->bill_type = strdup(argv[0]);
    }
    obd->gw_grp_id = atoi(argv[1]);
    obd->is_lcr_on = atoi(argv[2]);
    obd->rate_card_id = atoi(argv[3]);
    if (!IS_NULL(argv[4]) && strlen(argv[4]))
    {
        obd->call_plan_id = strdup(argv[4]);
    }
    if (!IS_NULL(argv[5]) && strlen(argv[5]))
    {
        obd->dial_prefix = strdup(argv[5]);
    }
    if (!IS_NULL(argv[6]) && strlen(argv[6]))
    {

        obd->buy_rate = strdup(argv[6]);
    }
    if (!IS_NULL(argv[7]) && strlen(argv[7]))
    {
        obd->sell_rate = strdup(argv[7]);
    }
    if (!IS_NULL(argv[8]) && strlen(argv[8]))
    {
        obd->min_dur = strdup(argv[8]);
    }
    if (!IS_NULL(argv[9]) && strlen(argv[9]))
    {
        obd->incr_dur = strdup(argv[9]);
    }
    if (!IS_NULL(argv[10]) && strlen(argv[10]))
    {

        obd->gw_id = strdup(argv[10]);
    }

    if (!IS_NULL(argv[11]) && strlen(argv[11]))
    {
        obd->gw_prepend_cid = strdup(argv[11]);
    }
    if (!IS_NULL(argv[12]) && strlen(argv[12]))
    {
        obd->gw_codec = strdup(argv[12]);
    }

    obd->call_hdr_typ = atoi(argv[13]);
    if (!IS_NULL(argv[14]) && strlen(argv[14]))
    {
        obd->clr_id = strdup(argv[14]);
    }
    if (!IS_NULL(argv[15]) && strlen(argv[15]))
    {
        obd->clr_id_hdr = strdup(argv[15]);
    }

    if (!IS_NULL(argv[16]) && strlen(argv[16]))
    {
        obd->clr_pfile = strdup(argv[16]);
    }

    obd->crdt_lmt = atof(argv[17]);
    obd->blnce = atof(argv[18]);
    obd->billing_typ = atoi(argv[19]);
    if (!IS_NULL(argv[20]) && strlen(argv[20]))
    {
        obd->is_mnt = strdup(argv[20]);
    }
    if (!IS_NULL(argv[21]) && strlen(argv[21]))
    {
        obd->talktime_mnt = strdup(argv[21]);
    }
    if (!IS_NULL(argv[22]) && strlen(argv[22]))
    {
        obd->call_plan_rate_id = strdup(argv[22]);
    }
    if (!IS_NULL(argv[23]) && strlen(argv[23]))
    {
        obd->area_code = strdup(argv[23]);
    }
    if (!IS_NULL(argv[24]) && strlen(argv[24]))
    {
        obd->strp_clr_id = strdup(argv[24]);
    }
    obd->gwty_status = atoi(argv[25]);

    return 0;
}

static int outbound_mnt_callback(void *parg, int argc, char **argv, char **column_names)
{
    mnt_detail_t *mnt = (mnt_detail_t *)parg;

    if (argc < 20)
    {
        switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, " Outbound_mnt callback  NO DATA %d\n", argc);
        return -1;
    }

    for (int i = 0; i < argc; i++)
        // switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, "exten : %d %s %s\n", i, column_names[i], argv[i]);

        mnt->is_init = true;
    if (!IS_NULL(argv[0]) && strlen(argv[0]))
    {
        mnt->pkg_blng_typ = strdup(argv[0]);
    }
    if (!IS_NULL(argv[1]) && strlen(argv[1]))
    {
        mnt->dialout_id = strdup(argv[1]);
    }
    if (!IS_NULL(argv[2]) && strlen(argv[2]))
    {
        mnt->dlout_cp_id = strdup(argv[2]);
    }
    if (!IS_NULL(argv[3]) && strlen(argv[3]))
    {
        mnt->dial_prefix = strdup(argv[3]);
    }
    if (!IS_NULL(argv[4]) && strlen(argv[4]))
    {
        mnt->gw_id = strdup(argv[4]);
    }

    if (!IS_NULL(argv[5]) && strlen(argv[5]))
    {
        mnt->gw_prepend_cid = strdup(argv[5]);
    }
    if (!IS_NULL(argv[6]) && strlen(argv[6]))
    {
        mnt->gw_codec = strdup(argv[6]);
    }
    if (!IS_NULL(argv[7]) && strlen(argv[7]))
    {
        mnt->gw_hdr_typ = strdup(argv[7]);
    }

    if (!IS_NULL(argv[8]) && strlen(argv[8]))
    {
        mnt->gw_caller_id = strdup(argv[8]);
    }

    if (!IS_NULL(argv[9]) && strlen(argv[9]))

    {
        mnt->gw_clr_id_hdr = strdup(argv[9]);
    }
    if (!IS_NULL(argv[10]) && strlen(argv[10]))
    {
        mnt->gw_id_prof = strdup(argv[10]);
    }
    if (!IS_NULL(argv[11]) && strlen(argv[11]))
    {
        mnt->is_mnt_plan = strdup(argv[11]);
    }
    if (!IS_NULL(argv[12]) && strlen(argv[12]))
    {
        mnt->remain_mnt = strdup(argv[12]);
    }
    mnt->is_group = atoi(argv[13]);

    if (!IS_NULL(argv[14]) && strlen(argv[14]))
    {
        mnt->group_id = strdup(argv[14]);
    }
    if (!IS_NULL(argv[15]) && strlen(argv[15]))
    {
        mnt->group_mnt = strdup(argv[15]);
    }
    if (!IS_NULL(argv[16]) && strlen(argv[16]))
    {
        mnt->strp_clr_id = strdup(argv[16]);
    }
    mnt->gwty_status = atoi(argv[17]);

    if (!IS_NULL(argv[18]) && strlen(argv[18]))
    {
        mnt->ext_minute = strdup(argv[18]);
    }
    if (!IS_NULL(argv[19]) && strlen(argv[19]))
    {
        mnt->ext_mnt_id = strdup(argv[19]);
    }
    return 0;
}

bool outbound(switch_core_session_t *session, char *dsn, switch_mutex_t *mutex, char *path, char *custom_path, call_details_t *call, const char *num)
{
    switch_channel_t *channel = switch_core_session_get_channel(session);

    if (clr_status(channel))
    {
        bool is_outbound_allowed = false;
        int type;
        float total_blance;
        char *cust_id = NULL;
        char *currnt_call = NULL;
        char is_call[20] = {'\0'};
        char esl_country_code[20] = {'\0'};
        char *tmp_num = NULL;
        char *query = NULL;
        char access_ip[20] = {'\0'};
        char *mnt_call = NULL;
        char *grp_mnt_call = NULL;
        char total_blnc[20];
        char *temp_path = NULL;
        cust_id = switch_mprintf("%s", switch_channel_get_variable(channel, "cust_id"));
        tmp_num = switch_mprintf("%s", num);

        switch_channel_set_variable(channel, "call_type", "outbound");
        switch_channel_set_variable(channel, "application", "outbound");

        currnt_call = switch_mprintf("call  verify_concurrent_call(%s)", cust_id);
        execute_sql2str(dsn, mutex, currnt_call, is_call, NELEMS(is_call));
        sleep(0.8);
        query = switch_mprintf("SELECT esl_country_code FROM `pbx_realtime_cdr` cdr \
    JOIN customer cust on cust.country_code != esl_country_code and cust.id = '%s' JOIN extension_master ext on ext.ext_number = '%s' and ext.roaming ='1'\
    WHERE cdr.uuid = '%s' order by cdr.id DESC;",
                               cust_id, switch_channel_get_variable(channel, "ani"), switch_channel_get_variable(channel, "call_uuid"));
        execute_sql2str(dsn, mutex, query, esl_country_code, NELEMS(esl_country_code));

        atoi(esl_country_code) != 0 ? call->flags.is_roaming = true : call->flags.is_roaming;

        switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_DEBUG, " esl_country_code :%s caller_is_on_roaming: %d", esl_country_code, call->flags.is_roaming);

        if (atoi(is_call) == 1)
        {
            temp_path = switch_mprintf("%s%s", path, "cc_limit_exhaust.wav");
            switch_ivr_play_file(session, NULL, temp_path, NULL);
            switch_safe_free(temp_path);
            switch_channel_set_variable(channel, "custom_hangup", "1007");
            switch_channel_hangup(channel, SWITCH_CAUSE_CALL_REJECTED);
        }

        (call->flags.is_frwd_outbound || call->did.is_outbound_on || call->flags.is_call_outbound) ? is_outbound_allowed = true : is_outbound_allowed;

        /* if (call->did.is_init){
        is_outbound_allowed =  call->callee.is_mnt_plan_enabled;
        } */

        (num[0] != '+') ? tmp_num = switch_mprintf("+%s", num) : tmp_num;
        switch_log_printf(SWITCH_CHANNEL_SESSION_LOG(session), SWITCH_LOG_DEBUG, "Dailed number :: %s", call->caller.dout_num);

        if ((call->caller.blacklist == true) && (is_black_list_outgoing(channel, dsn, mutex, call, tmp_num + (strlen(tmp_num) - 10))))
        {
            switch_log_printf(SWITCH_CHANNEL_SESSION_LOG(session), SWITCH_LOG_ERROR, "Dialed extension/Number is blacklisted.\n");
            temp_path = switch_mprintf("%s%s", path, "call_blacklist.wav");
            switch_ivr_play_file(session, NULL, temp_path, NULL);
            switch_safe_free(temp_path);
            switch_channel_hangup(channel, SWITCH_CAUSE_CALL_REJECTED);
            return 0;
        }

        query = switch_mprintf("call switch_access_restriction('%s','%s')", cust_id, switch_channel_get_variable(channel, "sip_h_X-userip"));
        execute_sql2str(dsn, mutex, query, access_ip, NELEMS(access_ip));
        switch_log_printf(SWITCH_CHANNEL_SESSION_LOG(session), SWITCH_LOG_DEBUG, "Access restriction :: %s  ,Minute Enable :: %d  ,sip_h_X-userip :: %s ", access_ip,
                          call->caller.is_mnt_plan_enabled, switch_channel_get_variable(channel, "sip_h_X-userip"));

        if ((is_outbound_allowed) && ((atoi(access_ip))))
        {

            mnt_call = switch_mprintf("call switch_minute_billing_query('%s','%s','%d','%d')", cust_id, call->caller.dout_num, call->flags.is_roaming, call->caller.id);
            switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, "Outbound Mintes :: %s ,Minute Dialout ID :: %s", mnt_call, call->mnt.dialout_id);
            execute_sql_callback(dsn, mutex, mnt_call, outbound_mnt_callback, &call->mnt);
            switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_DEBUG, "Minute :: %s , Minute initialize :: %d ", call->mnt.remain_mnt, call->mnt.is_init);
            switch_safe_free(mnt_call);

            if (call->mnt.is_init && (atoi(call->mnt.remain_mnt) > 0))
            {

                if (atoi(call->mnt.ext_minute) > 0)
                {
                    call->mnt.remain_mnt = call->mnt.ext_minute;
                    switch_channel_set_variable(channel, "is_ext_minute", "1");
                    switch_channel_set_variable(channel, "ext_mnt_id", call->mnt.ext_mnt_id);
                    switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, "Extension Minute Talking_mnt:%s\n", call->mnt.ext_minute);
                }
                else if (!atoi(call->mnt.ext_minute) && atoi(call->mnt.ext_mnt_id))
                {
                    switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, "Extension Minute EXHAUSTED ");
                    temp_path = switch_mprintf("%s%s", path, "balance_exhaust.wav");
                    switch_ivr_play_file(session, NULL, temp_path, NULL);
                    switch_safe_free(temp_path);
                    switch_channel_set_variable(channel, "custom_hangup", "1016");
                    switch_channel_hangup(channel, SWITCH_CAUSE_CALL_REJECTED);
                    return 0;
                }

                else
                {
                    call->obd.talktime_mnt = call->mnt.remain_mnt;
                }
                switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, "Minute Plan  talking_mnt:%s", call->mnt.remain_mnt);
                switch_channel_set_variable(channel, "talking_mnt", call->mnt.remain_mnt);
                switch_channel_set_variable(channel, "is_mnt_plan", "1");
                switch_channel_set_variable(channel, "call_plan_rate_mnt_id", call->mnt.dialout_id);
                switch_channel_set_variable(channel, "minute_plan_id", call->mnt.dlout_cp_id);
                switch_channel_set_variable(channel, "billing_type", "1");
                call->flags.is_outbound_mnt = true;
                return 1;
            }
            else
            {
                grp_mnt_call = switch_mprintf("call switch_grp_minute_query('%s','%s','%d',%d)", cust_id, call->caller.dout_num, call->flags.is_roaming, call->caller.id);
                switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, "Group minutes Check :: %s ,cust_id :: %s", grp_mnt_call, cust_id);
                execute_sql_callback(dsn, mutex, grp_mnt_call, outbound_mnt_callback, &call->mnt);
                switch_safe_free(grp_mnt_call);
                if (call->mnt.is_init && call->mnt.is_group && (atoi(call->mnt.group_mnt) > 0))
                {
                    call->flags.is_outbound_grp_mnt = true;
                    call->obd.talktime_mnt = call->mnt.group_mnt;
                    switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, "Group minute Available :: %d ,Talking minutes :: %s\n ", call->mnt.is_group, call->mnt.group_mnt);
                    switch_channel_set_variable(channel, "is_group_mnt_plan", "1");
                    switch_channel_set_variable(channel, "talking_mnt", call->mnt.group_mnt);
                    switch_channel_set_variable(channel, "call_plan_rate_mnt_id", call->mnt.group_id);
                    switch_channel_set_variable(channel, "minute_plan_id", call->mnt.dlout_cp_id);
                    switch_channel_set_variable(channel, "billing_type", "2");
                    return 1;
                }
            }

            if (!(call->flags.is_roaming))
            {

                query = switch_mprintf("call switch_billing_query('%s','%s','%s')", cust_id, call->caller.dout_num, call->caller.dout_num);
                switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, "Outbound :: %s", query);
                execute_sql_callback(dsn, mutex, query, outbound_callback, &call->obd);
                switch_safe_free(query);
                switch_channel_set_variable(channel, "talking_mnt", "0");
                switch_channel_set_variable(channel, "billing_type", "3");

                if (call->obd.is_init)
                {
                    if (call->obd.billing_typ == 1)
                        type = 0;
                    else
                        type = 1;
                    total_blance = call->obd.blnce + call->obd.crdt_lmt * type;
                    sprintf(total_blnc, "%f", total_blance);
                    switch_channel_set_variable(channel, "total_balance", total_blnc);
                    switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, "Outbound, Total Available Balance ::%f  ,Sell Rate: %f ", total_blance, atof(call->obd.sell_rate));
                    if (total_blance < atof(call->obd.sell_rate))
                    {
                        temp_path = switch_mprintf("%s%s", path, "balance_exhaust.wav");
                        switch_ivr_play_file(session, NULL, temp_path, NULL);
                        switch_safe_free(temp_path);
                        switch_channel_set_variable(channel, "custom_hangup", "1003");
                        switch_channel_hangup(channel, SWITCH_CAUSE_CALL_REJECTED);
                        return 0;
                    }
                }
            }

            /*  else{
            switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, "You are in roaming and don't have any active plan. Please contact the administrator \n");
            } */
        }

        else if (!(atoi(access_ip)))
        {
            switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, "Your ip is blocked plz contact your admin");
            temp_path = switch_mprintf("%s%s", path, "ip_blocked.wav");
            switch_ivr_play_file(session, NULL, temp_path, NULL);
            switch_safe_free(temp_path);
            switch_channel_hangup(channel, SWITCH_CAUSE_CALL_REJECTED);
            return 0;
        }
        else
        {
            temp_path = switch_mprintf("%s%s", path, "balance_exhaust.wav");
            switch_ivr_play_file(session, NULL, temp_path, NULL);
            switch_safe_free(temp_path);
            switch_channel_hangup(channel, SWITCH_CAUSE_CALL_REJECTED);
            return 0;
        }
    }
    // switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, "Outbound : %d", call->obd.is_init);
    return true;
}

static int did_init_callback(void *parg, int argc, char **argv, char **column_names)
{
    did_details_t *did = (did_details_t *)parg;
    char *tmp;
    if (argc < 23)
    {
        switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, " DID CALLBACK  NO DATA %d\n", argc);
        return -1;
    }
    for (int i = 0; i < argc; i++)
        // switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, "DID %d %s %s\n", i, column_names[i], argv[i]);

        if (!IS_NULL(argv[0]) && strlen(argv[0]))
        {

            did->cust_id = strdup(argv[0]);
            did->is_init = true;
        }
    memcpy(did->num, argv[1], strlen(argv[1])); // did_no
    if (!IS_NULL(argv[2]) && strlen(argv[2]))
    {
        did->bill_type = strdup(argv[2]);
    }
    did->fixrate = strtod(argv[3], &tmp); // add tmp check via macro / func if error abort
    if (!IS_NULL(argv[4]) && strlen(argv[4]))
    {
        did->conn_charge = strdup(argv[4]); // add tmp check via macro / func if error abort
    }

    if (!IS_NULL(argv[5]) && strlen(argv[5]))
    {

        did->selling_rate = strdup(argv[5]); // add tmp check via macro / func if error abort
    }
    did->max_cc = atoi(argv[6]);
    did->type = atoi(argv[7]);
    did->is_blacklist_on = atoi(argv[8]);
    did->is_vm_on = atoi(argv[9]);
    did->is_recording_on = atoi(argv[10]);
    did->is_outbound_on = atoi(argv[11]);
    did->is_call_barging_on = atoi(argv[12]);
    did->actv_ftr_id = atoi(argv[13]);
    did->dst_id = atoi(argv[14]);
    did->tm_gp_id = atoi(argv[15]);
    did->id = atoi(argv[16]);
    did->crdt_lmt = atoi(argv[17]);
    did->blnce = atoi(argv[18]);
    did->billing_typ = atoi(argv[19]);
    did->tm_grp_failover = atoi(argv[20]);
    did->geo_tracking = atoi(argv[21]);
    if (!IS_NULL(argv[22]) && strlen(argv[22]))
    {
        strcpy(did->country_prefix, argv[22]);
    }

    if (!IS_NULL(argv[23]) && strlen(argv[23]))
    {
    did->call_back_url = strdup(argv[23]);
    }


    return 0;
}

static int did_mnt_billing_callback(void *parg, int argc, char **argv, char **column_names)
{

    did_mnt_billing *did_mnt = (did_mnt_billing *)parg;

    for (int i = 0; i < argc; i++)
        // switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, "DID Mnt billing %d %s %s\n", i, column_names[i], argv[i]);
        if (!IS_NULL(argv[0]) && strlen(argv[0]))
        {

            did_mnt->cpr_id = strdup(argv[0]);
            did_mnt->is_init = true;
        }

    if (!IS_NULL(argv[1]) && strlen(argv[1]))
    {
        did_mnt->is_grp = strdup(argv[1]);
    }

    if (!IS_NULL(argv[2]) && strlen(argv[2]))
    {
        did_mnt->group_id = strdup(argv[2]);
    }
    if (!IS_NULL(argv[3]) && strlen(argv[3]))
    {
        did_mnt->call_plan = strdup(argv[3]);
    }
    if (!IS_NULL(argv[4]) && strlen(argv[4]))
    {
        did_mnt->minute = strdup(argv[4]);
    }

    if (!IS_NULL(argv[5]) && strlen(argv[5]))
    {

        did_mnt->bundle_plan_id = strdup(argv[5]);
    }

    return 0;
}


bool verify_did(switch_channel_t *channel, char *dsn, switch_mutex_t *mutex, char *path,char *custom_path, call_details_t *call)
{
    char *sql = NULL;
    char did_no[20] = {'\0'};
    char total_balance[20] = {'\0'};
    switch_core_session_t *session = switch_channel_get_session(channel);
    const char *callee = switch_channel_get_variable(channel, "destination_number");
    switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, " callee:%s", callee);

    sql = switch_mprintf("call switch_call_didno('%s')", callee);
    execute_sql_callback(dsn, mutex, sql, did_init_callback, &call->did);
    switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, " DID CALLBACK  sql: %s  DID route id:%d %d", sql, call->did.actv_ftr_id,call->did.is_init);

    if (call->did.actv_ftr_id==0 && call->did.is_init )
     {

            switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, "DID not routed :  %d", call->did.is_init);
            switch_channel_set_variable(channel, "custom_hangup", "1018");
            switch_core_session_execute_application(session, "playback", switch_mprintf("%s%s", path, "did_feature_not_routed.wav"));
            switch_channel_hangup(channel, SWITCH_CAUSE_CALL_REJECTED);
            return true;
     }
    else if (call->did.is_init)
     {

       sql = switch_mprintf("call switch_did_minute_check('%s','%s')", call->did.cust_id, call->did.country_prefix);
       execute_sql_callback(dsn, mutex, sql, did_mnt_billing_callback, &call->did_mnt);
       switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, " DID CALLBACK  sql: %s did_mnt_billing %d  ", sql, call->did_mnt.is_init);
       switch_channel_set_variable(channel, "cust_id", call->did.cust_id);
       switch_channel_set_variable(channel, "did_billing_type", call->did.bill_type);
       switch_channel_set_variable(channel, "dial_prefix", call->did.country_prefix);

       if (call->did_mnt.is_init)
           {
            switch_channel_set_variable(channel, "is_mnt_plan", "1");
            switch_channel_set_variable(channel, "did_mnt", "1");
            if (atoi(call->did_mnt.is_grp) == 1)
            {
                switch_channel_set_variable(channel, "is_group_mnt_plan", "1");
                switch_channel_set_variable(channel, "billing_type", "2");
                switch_channel_set_variable(channel, "call_plan_rate_mnt_id", call->did_mnt.group_id);
            }
            else
            {
                switch_channel_set_variable(channel, "billing_type", "1");
                switch_channel_set_variable(channel, "call_plan_rate_mnt_id", call->did_mnt.cpr_id);
            }

            switch_channel_set_variable(channel, "talking_mnt", call->did_mnt.minute);
            switch_channel_set_variable(channel, "minute_plan_id", call->mnt.dlout_cp_id);
        }
      else
        {
            switch_channel_set_variable(channel, "did_connect_charge", call->did.conn_charge);
            switch_channel_set_variable(channel, "sell_rate", call->did.selling_rate);
            switch_channel_set_variable(channel, "did_sell_rate", call->did.selling_rate);
            switch_channel_set_variable(channel, "selling_billing_block", "60");
            switch_channel_set_variable(channel, "billing_type", call->did.bill_type);
            sprintf(total_balance, "%d", call->did.blnce);
            switch_channel_set_variable(channel, "total_balance", total_balance);
       }

        switch_safe_free(sql);
    
       sprintf(did_no, "%d", call->did.geo_tracking);
       switch_channel_set_variable(channel, "geo_location", did_no);
       handle_did_dest(channel, dsn, mutex, path, custom_path, call);
     }
     
    return (call->did.is_init) ? true : false;
}

int conf_init_callback(void *parg, int argc, char **argv, char **column_names)
{
    conf_details_t *conf = (conf_details_t *)parg;
    if (argc < 14)
    {
        switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, " CONF CALLBACK  NO DATA %d\n", argc);
        return -1;
    }
    for (int i = 0; i < argc; i++)
        // switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, "INTERNAL CONF %d %s %s\n", i, column_names[i], argv[i]);

        conf->is_init = true;
    if (!IS_NULL(argv[0]) && strlen(argv[0]))
        conf->id = strdup(argv[0]);
    if (!IS_NULL(argv[1]) && strlen(argv[1]))
        conf->conf_name = strdup(argv[1]);
    if (!IS_NULL(argv[2]) && strlen(argv[2]))
        conf->ext = strdup(argv[2]);
    if (!IS_NULL(argv[3]) && strlen(argv[3]))
        conf->cust_id = strdup(argv[3]);
    if (!IS_NULL(argv[4]) && strlen(argv[4]))
        conf->participant_id = strdup(argv[4]);
    if (!IS_NULL(argv[5]) && strlen(argv[5]))
        conf->admin_pin = strdup(argv[5]);
    if (!IS_NULL(argv[6]) && strlen(argv[6]))
        conf->mem_pin = strdup(argv[6]);

    conf->moh = atoi(argv[7]);
    conf->is_recording = atoi(argv[8]);
    if (!IS_NULL(argv[9]) && strlen(argv[9]))
        conf->wc_prompt = strdup(argv[9]);
    conf->wait_moderator = atoi(argv[10]);
    conf->name_record = atoi(argv[11]);
    conf->end_conf = atoi(argv[12]);
    conf->feature_enabled = atoi(argv[13]);

    return 0;
}

static int cg_init_callback(void *parg, int argc, char **argv, char **column_names)
{
    callgrp_details_t *cg = (callgrp_details_t *)parg;
    if (argc < 9)
    {
        switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, " CG CALLBACK NO DATA %d\n", argc);
        return -1;
    }
    for (int i = 0; i < argc; i++)
        // switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, "INTERNAL CG %d %s %s\n", i, column_names[i], argv[i]);

        cg->is_init = true;
    if (!IS_NULL(argv[0]) && strlen(argv[0]))
    {
        cg->id = strdup(argv[0]);
    }
    cg->grp_type = atoi(argv[1]);
    cg->is_recording_on = atoi(argv[2]);
    cg->ring_timeout = atoi(argv[3]);
    if (!IS_NULL(argv[4]) && strlen(argv[4]))
    {
        cg->extensions = strdup(argv[4]);
    }
    cg->sticky_agent = atoi(argv[5]);
    cg->un_failover = atoi(argv[6]);

    if (!IS_NULL(argv[7]) && strlen(argv[7]))
    {
        cg->wc_pmt_path = strdup(argv[7]);
    }
    cg->feature_enabled = atoi(argv[8]);
    return 0;
}

static int fmfm_init_callback(void *parg, int argc, char **argv, char **column_names)
{
    fmfm_details_t *fmfm = (fmfm_details_t *)parg;
    if (argc < 5)
    {
        switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, " fmfm CALLBACK  NO DATA %d\n", argc);
        return -1;
    }
    for (int i = 0; i < argc; i++)
        // switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, "INTERNAL fmfm %d %s %s\n", i, column_names[i], argv[i]);

        fmfm->is_init = true;
    if (!IS_NULL(argv[0]) && strlen(argv[0]))
    {
        fmfm->ring_timeout = strdup(argv[0]);
    }

    fmfm->id = atoi(argv[1]);
    if (!IS_NULL(argv[2]) && strlen(argv[2]))
    {
        fmfm->fmfm1 = strdup(argv[2]);
    }
    if (!IS_NULL(argv[3]) && strlen(argv[3]))
    {
        fmfm->fmfm2 = strdup(argv[3]);
    }
    if (!IS_NULL(argv[4]) && strlen(argv[4]))
    {
        fmfm->fmfm3 = strdup(argv[4]);
    }
    return 0;
}

void handle_fmfm(switch_channel_t *channel, char *dsn, switch_mutex_t *mutex, char *path, char *custom_path, call_details_t *call)
{
    switch_core_session_t *session = switch_channel_get_session(channel);
    char *query;
    const char *dialstatus;
    char *temp_path;
    bool pstn_dial_out = false;
    bool fmfm_enable = false;
    char contact[17] = {"\0"};
    const char *ext_id = switch_channel_get_variable(channel, "ext_id");

    query = switch_mprintf("call switch_handle_fmfm(%d)", call->callee.id);
    //(call->caller.is_outbound_allowed == 0) ? call->caller.is_outbound_allowed  = call->caller.is_mnt_plan_enabled :call->caller.is_outbound_allowed ;
    switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_DEBUG, " fmfm SQL : %s ext_id:%s  is outbound :%d", query, ext_id, call->caller.is_outbound_allowed);
    execute_sql_callback(dsn, mutex, query, fmfm_init_callback, &call->fmfm);

    if (call->fmfm.is_init)
    {
        switch_channel_set_variable(channel, "call_type", "fmfm");
        switch_channel_set_variable(channel, "dial_tm_out", call->fmfm.ring_timeout);
        fmfm_enable = (atoi(call->fmfm.fmfm1) || atoi(call->fmfm.fmfm2) || atoi(call->fmfm.fmfm3));

        if (fmfm_enable)
        {
            temp_path = switch_mprintf("%s%s", path, "call_forwarding.wav");
            switch_ivr_play_file(session, NULL, temp_path, NULL);
            switch_safe_free(temp_path);
        }

        dialstatus = switch_channel_get_variable(channel, "sip_term_cause");
        switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, " sip_term_cause : %s \n", dialstatus);

        !(IS_NULL(dialstatus)) ? call->flags.isCallerHangup = strcmp(dialstatus, "487") : call->flags.isCallerHangup;
        switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, " flag  : %d \n", call->flags.isCallerHangup);

        for (size_t i = 1; i <= 3 && call->flags.isCallerHangup; i++)
        {
            if (i == 1)
            {
                strcpy(contact, call->fmfm.fmfm1);
            }
            else if (i == 2)
            {
                strcpy(contact, call->fmfm.fmfm2);
            }
            else
            {
                strcpy(contact, call->fmfm.fmfm3);
            }

            switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_WARNING, " contact %zu : %s \n", i, contact);

            if (strcmp(contact, ""))
            {

                if (contact[0] == '+')
                {
                    call->obd.is_init = call->mnt.is_init = false;
                    call->flags.is_call_outbound = true;

                    dialoutrule(channel, dsn, mutex, path, custom_path, call, contact);
                    outbound(session, dsn, mutex, path, custom_path, call, contact);
                    if ((call->obd.is_init || call->mnt.is_init) && call->drule.is_init)
                    {
                        bridge_call(session, call, contact, dsn, mutex);
                    }
                }
                else
                {
                    call->flags.is_call_outbound = false;
                    switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, " sip : %s\n", contact);
                    bridge_call(session, call, contact, dsn, mutex);
                }
                dialstatus = switch_channel_get_variable(channel, "last_bridge_proto_specific_hangup_cause");
                switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, " cause : %s Call_status %d\n", dialstatus, call->flags.isCallerHangup);

                if (dialstatus != NULL && !strcmp(dialstatus, "sip:200"))
                {
                    pstn_dial_out = true;
                    switch_channel_hangup(channel, SWITCH_CAUSE_CALL_REJECTED);
                }
                else if (!(call->flags.isCallerHangup))
                {
                    break;
                }
                else
                {
                    call->drule.is_init = false;
                }
            }
        }

        if (!pstn_dial_out && fmfm_enable)
        {
            sleep(1);
            temp_path = switch_mprintf("%s%s", path, "FMFM_no_user_avail.wav");
            switch_ivr_play_file(session, NULL, temp_path, NULL);
            switch_safe_free(temp_path);
        }

        switch_safe_free(query);
    }
}

bool verify_internal_exten(switch_channel_t *channel, char *dsn, switch_mutex_t *mutex, call_details_t *call, const char *num, char *path)
{
    char *sql, *temp_path = NULL;
    const char *cg_active_id = switch_channel_get_variable(channel, "cg_active_id");
    const char *cust_id = switch_channel_get_variable(channel, "cust_id");
    bool res = false;
    switch_core_session_t *session = switch_channel_get_session(channel);

    (!IS_NULL(cg_active_id) && !IS_NULL(cust_id)) ? call->did.dst_id = atoi(cg_active_id) : call->did.dst_id;
    (IS_NULL(cust_id)) ? cust_id = call->caller.cust_id : cust_id;

    // switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, " CG cg_active_id : %s cust_id:%s  call_type :%s  did::%d", cg_active_id,cust_id,switch_channel_get_variable(channel,"call_type"),call->did.is_init);
    sql = switch_mprintf("call switch_call_group(%s,%d,%s)", cust_id, call->did.dst_id, num);
    switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, "CG ::%s", sql);
    execute_sql_callback(dsn, mutex, sql, cg_init_callback, &call->cg);
    switch_safe_free(sql);
    res = call->cg.is_init;
    switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, "CG initialized :: %d", res);

    if (!res)
    {
        if (call->did.is_init)
        {
            call->caller.cust_id = call->did.cust_id;
            num = switch_mprintf("%d", call->did.dst_id);
        }
        sql = switch_mprintf("call switch_confrence_details(%s,%s)", call->caller.cust_id, num);
        switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, " Conference SQL : %s", sql);
        execute_sql_callback(dsn, mutex, sql, conf_init_callback, &call->conf);
        switch_safe_free(sql);
        res = call->conf.is_init;
        call->cg.feature_enabled = call->conf.feature_enabled;

        switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, " Conference res : %d", res);

        if (res == 0)
        {
            sleep(1);
            temp_path = switch_mprintf("%s%s", path, "no_conf_available.wav");
            switch_ivr_play_file(session, NULL, temp_path, NULL);
            switch_safe_free(temp_path);
            switch_channel_hangup(channel, SWITCH_CAUSE_CALL_REJECTED);
        }
    }
    if (res && !call->cg.feature_enabled)
    {
        switch_channel_set_variable(channel, "custom_hangup", "1018");
        temp_path = switch_mprintf("%s%s", path, "ivr-not_have_permission.wav");
        switch_core_session_execute_application(session, "playback", temp_path);
        switch_channel_hangup(channel, SWITCH_CAUSE_CALL_REJECTED);
        return false;
    }
    return res;
}

void eaves_drop(switch_core_session_t *session, const char *extension, call_details_t *call_info, char *dsn, switch_mutex_t *mutex,char *path, char *custom_path)
{
    switch_channel_t *channel = switch_core_session_get_channel(session);
    if (clr_status(channel))
    {

        char *tmp_str = NULL;
        char uuid[200] = {'\0'};
        switch_channel_t *channel = switch_core_session_get_channel(session);
        switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, "callforwording statusf :%s", extension);
        switch_channel_answer(channel);
        tmp_str = switch_mprintf("SELECT uuid  FROM `pbx_realtime_cdr` WHERE customer_id = '%s' AND (CONCAT(customer_id, '%s') IN (forward, src) OR '%s' IN (forward, src)) \
        AND current_status != 'CHANNEL_HANGUP' LIMIT 1;",call_info->caller.cust_id, extension,extension);
        execute_sql2str(dsn, mutex, tmp_str, uuid, NELEMS(uuid));
       // switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, "query %s :\n", tmp_str);
        
        if (strcmp(uuid, ""))
            switch_core_session_execute_application(session, "eavesdrop", uuid);
        else
        {
            switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, "dial %s  agent number not on any  call :\n", extension);
            switch_core_session_execute_application(session, "playback",switch_mprintf("%s%s", path, "eavesdrop_noagent.wav"));
            switch_channel_hangup(channel, SWITCH_CAUSE_CALL_REJECTED);
        }
    }
}

static int fd_init_callback(void *parg, int argc, char **argv, char **column_names)
{
    feture_details_t *fd = (feture_details_t *)parg;
    if (argc < 3)
    {
        switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, " fd CALLBACK NO DATA %d\n", argc);
        return -1;
    }
    for (int i = 0; i < argc; i++)
        // switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, "INTERNAL fd %d %s %s\n", i, column_names[i], argv[i]);

        fd->is_init = true;
    fd->bling_typ = atoi(argv[0]);
    if (!IS_NULL(argv[1]) && strlen(argv[1]))
    {
        fd->blnce = strdup(argv[1]);
    }
    if (!IS_NULL(argv[2]) && strlen(argv[2]))
    {
        fd->crdt_lmt = strdup(argv[2]);
    }

    return 0;
}

void feature_code(switch_core_session_t *session, const char *callee, const char *caller, call_details_t *call_info, char *dsn, switch_mutex_t *mutex, char *path, char *custom_path)
{
    char *temp_path = NULL;
    char *query = NULL;
    char *call_frwd = NULL;
    char result[128] = {0};
    char *audio_file = NULL;
    uint8_t min = 2;
    uint8_t max = 14;
    char digit_buffer[128];
    char *ivr_pin;
    char *hr, *mnt;
    char *tmp = NULL;
    char *PSTN = NULL;
    char digit[128] = "1";
    char *pin;
    char *chck_blnc;
    char *chck_invce;
    char *tmp_invce;
    char *tmp_blnce;
    char invce[20] = {'\0'};
    char status[50] = {0};
    char token_list[20][20];
    int num_tokens = 0;
    char *context = NULL;
    char *token = NULL;
    char *digits_regex = {'\0'};
    int32_t max_tries = 3;

    float crdt_lmt, blnce, total_blnce;
    int i, tries = 0;
    bool ext = 0;

    switch_channel_t *channel = switch_core_session_get_channel(session);

    switch_channel_answer(channel);

    audio_file = switch_mprintf("%s%s", path, "speech.wav");
    tmp = switch_mprintf("%s%s", path, "check_blnc.wav");
    switch (atoi(callee + 1))
    {
    case 12:
        // switch_channel_set_variable(channel,"call_type","call_enable_dnd");
        switch_channel_answer(channel);
        if (call_info->caller.is_admin)
        {
            sleep(2);
            for (i = 0; i < 3; i++)
            {
                switch_play_and_get_digits(session, 1, 1, 1, 10000, "#", tmp, NULL, NULL, digit, sizeof(digit), "[0-9]+", 10000, NULL);
                pin = strdup(digit);

                switch (atoi(pin))
                {
                case 1:
                    ext = 1;
                    chck_blnc = switch_mprintf("SELECT billing_type,balance,credit_limit FROM `customer` where id=%s", call_info->caller.cust_id);
                    switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, " fd SQL : %s\n", chck_blnc);
                    execute_sql_callback(dsn, mutex, chck_blnc, fd_init_callback, &call_info->fd);
                    if (call_info->fd.bling_typ == 2)
                    {
                        sscanf(call_info->fd.blnce, "%f", &blnce);
                        sscanf(call_info->fd.crdt_lmt, "%f", &crdt_lmt);
                        total_blnce = blnce + crdt_lmt;
                        tmp_blnce = switch_mprintf("en CURRENCY PRONOUNCED %f", total_blnce);
                    }
                    else
                    {
                        tmp_blnce = switch_mprintf("en CURRENCY PRONOUNCED %s", call_info->fd.blnce);
                    }
                    switch_core_session_execute_application(session, "say", tmp_blnce);
                    switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, "pin: %s\n", pin);
                    switch_safe_free(chck_blnc);
                    // switch_channel_hangup(channel,SWITCH_CAUSE_CALL_REJECTED);

                    break;
                case 2:
                    ext = 1;
                    chck_invce = switch_mprintf("SELECT amount_with_gst FROM `pbx_invoice` where customer_id=%s order by id desc limit 1", call_info->caller.cust_id);
                    execute_sql2str(dsn, mutex, chck_invce, invce, NELEMS(invce));
                    switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, "invoice: %s\n", invce);
                    tmp_invce = switch_mprintf("en CURRENCY PRONOUNCED %s", invce);
                    switch_core_session_execute_application(session, "say", tmp_invce);
                    switch_safe_free(chck_invce);
                    switch_safe_free(tmp_invce);
                    // switch_channel_hangup(channel,SWITCH_CAUSE_CALL_REJECTED);

                    break;
                default:
                    temp_path = switch_mprintf("%s%s", path, "invalid_digit.wav");
                    switch_ivr_play_file(session, NULL, temp_path, NULL);
                    switch_safe_free(temp_path);
                    // switch_channel_hangup(channel,SWITCH_CAUSE_CALL_REJECTED);

                    break;
                }
                if (ext)
                {
                    temp_path = switch_mprintf("%s%s", path, "thankyou_selfcare.wav");
                    switch_ivr_play_file(session, NULL, temp_path, NULL);
                    switch_safe_free(temp_path);
                    switch_channel_hangup(channel, SWITCH_CAUSE_CALL_REJECTED);
                }
            }
            temp_path = switch_mprintf("%s%s", path, "thankyou_selfcare.wav");
            switch_ivr_play_file(session, NULL, temp_path, NULL);
            switch_safe_free(temp_path);
            switch_channel_hangup(channel, SWITCH_CAUSE_CALL_REJECTED);
        }
        else
        {
            switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, " No admin permission %d :\n", call_info->caller.is_admin);
            sleep(2);
            temp_path = switch_mprintf("%s%s", path, "not_authorized_to_dial.wav");
            switch_ivr_play_file(session, NULL, temp_path, NULL);
            switch_safe_free(temp_path);
            switch_channel_hangup(channel, SWITCH_CAUSE_CALL_REJECTED);
        }
        break;

    case 33:
        switch_channel_set_variable(channel, "call_type", "call_evesdrop");
        if (call_info->caller.is_admin)
        {
            ivr(session, min, max, audio_file, result, call_info, dsn, mutex,path,custom_path);
        }
        else
        {
            switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, " No admin permission %d :\n", call_info->caller.is_admin);
            sleep(1);
            temp_path = switch_mprintf("%s%s", path, "not_authorized_to_dial.wav");
            switch_core_session_execute_application(session, "playback", temp_path);
            switch_safe_free(temp_path);
            switch_channel_hangup(channel, SWITCH_CAUSE_CALL_REJECTED);
        }

        break;

    /*  call alram new feature added*/
    case 50:

        switch_channel_set_variable(channel, "call_type", "call_alarm");
        switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, "call_alarm");

        /*     query = switch_mprintf(" SELECT src FROM `pbx_call_alarm` WHERE src = '%s' and date_slot in (CURRENT_DATE+1 , CURRENT_DATE)",caller);
        execute_sql2str(dsn, mutex, query, status, NELEMS(status));
        switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, "Exten query: %d\n", atoi(status)); */

        /*    if (atoi(status) == 0){ */

        while (tries < 3)
        {
            audio_file = switch_mprintf("%s%s", path, "hrs_alarm.wav");
            switch_play_and_get_digits(session, 1, 2, 1, 15000, "#", audio_file, NULL, NULL, digit_buffer,
                                       sizeof(digit_buffer), "[0-9]+", 2000, NULL);
            hr = strdup(digit_buffer);
            switch_safe_free(audio_file);

            switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, "hr time ::%s tries %d", hr, tries);
            if (atoi(hr) <= 23)
            {
                switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, "setting hr time ::%s", hr);
                audio_file = switch_mprintf("%s%s", path, "mnt_alarm.wav");
                switch_play_and_get_digits(session, 1, 2, 1, 15000, "#", audio_file, NULL, NULL, digit_buffer,
                                           sizeof(digit_buffer), "[0-9]+", 2000, NULL);
                mnt = strdup(digit_buffer);
                switch_safe_free(audio_file);

                if (atoi(mnt) <= 59)
                {
                    switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, " setting min ::%s", mnt);
                }
                else
                {
                    switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, " Error making min ::%s", mnt);
                    switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, "error making hr ::%s", hr);
                    temp_path = switch_mprintf("%s%s", path, "invalid_digit.wav");
                    switch_ivr_play_file(session, NULL, temp_path, NULL);
                    switch_safe_free(temp_path);
                }

                if (atoi(mnt) != 0)
                {
                    query = switch_mprintf("INSERT INTO pbx_call_alarm (`src`,`cust_id`,`date_slot`, `time_start`) VALUES \
    ('%s','%s',CURRENT_DATE,'%s:%s')",
                                           caller, call_info->caller.cust_id, hr, mnt);
                    execute_sql(dsn, query, mutex);
                    switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, " query %s \n Final time  %s:%s ", query, hr, mnt);

                    switch_safe_free(query);
                    switch_channel_set_variable(channel, "DIALSTATUS", "SUCCESS");
                }

                temp_path = switch_mprintf("%s%s", path, "thankyou_selfcare.wav");
                switch_ivr_play_file(session, NULL, temp_path, NULL);
                switch_safe_free(temp_path);
                switch_channel_hangup(channel, SWITCH_CAUSE_CALL_REJECTED);
            }
            else
            {
                switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, "error making hr ::%s", hr);
                temp_path = switch_mprintf("%s%s", path, "invalid_digit.wav");
                switch_ivr_play_file(session, NULL, temp_path, NULL);
                switch_safe_free(temp_path);
                switch_channel_set_variable(channel, "DIALSTATUS", "");
            }

            tries++;
        }

        break;

    case 69:

        switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, "caller: %s  callee:%s\n", caller, callee);
        query = switch_mprintf("SELECT GROUP_CONCAT(src) FROM ( SELECT DISTINCT(src) FROM `pbx_realtime_cdr` WHERE dst ='%s' or forward ='%s' \
    AND terminatecause = '487' ORDER BY `pbx_realtime_cdr`.`id` LIMIT 3) as src;",
                               caller, caller);
        execute_sql2str(dsn, mutex, query, status, NELEMS(status));
        // switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, " *69 feature last call by caller query : %s\n  %s", query, status);

        token = strtok_r(status, ",", &context);

        while (token != NULL)
        {
            strcpy(token_list[num_tokens], token);
            num_tokens++;
            token = strtok_r(NULL, ",", &context);
        }

        switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, " token_list : %s   num_tokens %d", token_list[0], num_tokens);

        digits_regex = switch_mprintf("[1-%d]+", num_tokens);
        switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, "digits_regex :%s", digits_regex);

        if (num_tokens >= 1)
        {

            for (size_t i = 0; i <= max_tries; i++)
            {

                if (num_tokens == 1 || num_tokens == 2 || num_tokens == 3)
                {
                    sleep(1);
                    switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, "wel_69.wav ");
                    audio_file = switch_mprintf("%s%s", path, "wel_69.wav");
                    switch_ivr_play_file(session, NULL, audio_file, NULL);
                    tmp = switch_mprintf("%s%s", path, "press_1.wav");
                    switch_ivr_play_file(session, NULL, tmp, NULL);

                    temp_path = switch_mprintf("en name_spelled iterated  %s", token_list[0]);
                    switch_core_session_execute_application(session, "say", temp_path);
                }
                if (num_tokens == 2 || num_tokens == 3)
                {

                    tmp = switch_mprintf("%s%s", path, "press_2.wav");
                    switch_ivr_play_file(session, NULL, tmp, NULL);
                    temp_path = switch_mprintf("en name_spelled iterated  %s", token_list[1]);
                    switch_core_session_execute_application(session, "say", temp_path);
                }
                if (num_tokens == 3)
                {

                    tmp = switch_mprintf("%s%s", path, "press_3.wav");
                    switch_ivr_play_file(session, NULL, tmp, NULL);
                    temp_path = switch_mprintf("en name_spelled iterated  %s", token_list[2]);
                    switch_core_session_execute_application(session, "say", temp_path);
                }

                switch_safe_free(temp_path);
                switch_safe_free(tmp);

                switch_play_and_get_digits(session, 1, 1, 1, 15000, "#", NULL, switch_mprintf("%s%s", path, "no_input.wav"), NULL, digit_buffer,
                                           sizeof(digit_buffer), digits_regex, 2000, NULL);
                ivr_pin = strdup(digit_buffer);
                switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, " ivr_pin : %d  max_tries:%d callee to dial  : %s\n ", atoi(ivr_pin), max_tries, token_list[atoi(ivr_pin) - 1]);

                if (strlen(token_list[atoi(ivr_pin) - 1]) > 8)
                {
                    if (token_list[atoi(ivr_pin) - 1][0] != '+')
                    {
                        PSTN = switch_mprintf("+%s", token_list[atoi(ivr_pin) - 1]);
                    }
                    else
                    {
                        PSTN = token_list[atoi(ivr_pin) - 1];
                    }

                    switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, " PSTN Number::: %s is_sign there:%c", PSTN, PSTN[0]);

                    temp_path = switch_mprintf("en name_spelled pronounced   %s", PSTN);
                    switch_core_session_execute_application(session, "say", temp_path);

                    switch_play_and_get_digits(session, 1, 1, 1, 15000, "#", switch_mprintf("%s%s", path, "press_1_2_continue_69.wav"), switch_mprintf("%s%s", path, "no_input.wav"), NULL, digit_buffer,
                                               sizeof(digit_buffer), "[0-1]+", 2000, NULL);
                    ivr_pin = strdup(digit_buffer);
                    if (atoi(ivr_pin) == 1 && PSTN[0] == '+')
                    {

                        call_info->flags.is_call_outbound = true;
                        switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, " Token: %s\n", PSTN);
                        dialoutrule(channel, dsn, mutex, path, custom_path, call_info, PSTN);
                        outbound(session, dsn, mutex, path, custom_path, call_info, PSTN);
                        if (call_info->obd.is_init || call_info->flags.is_outbound_mnt || call_info->flags.is_outbound_grp_mnt)
                        {
                            bridge_call(session, call_info, PSTN, dsn, mutex);
                            switch_channel_hangup(channel, SWITCH_CAUSE_CALL_REJECTED);
                        }
                    }
                    else if ((atoi(ivr_pin) == 0))
                    {
                        switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_DEBUG, " GO back previous menu:::%d counter: %zu", atoi(ivr_pin), i);
                    }
                }

                else if (strlen(token_list[atoi(ivr_pin) - 1]) >= 1 && (get_ext_details(channel, &call_info->callee, dsn, mutex, token_list[atoi(ivr_pin) - 1])))
                {
                    switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, "SIP callee:::%s", token_list[atoi(ivr_pin) - 1]);

                    if (!strcmp(token_list[atoi(ivr_pin) - 1], caller))
                    {

                        temp_path = switch_mprintf("%s%s", path, "not_authorized_to_dial.wav");
                        switch_ivr_play_file(session, NULL, temp_path, NULL);
                        switch_safe_free(temp_path);
                        switch_channel_hangup(channel, SWITCH_CAUSE_CALL_REJECTED);
                    }

                    switch_channel_set_variable(channel, "call_type", "sip_extn");
                    temp_path = switch_mprintf("%s%s", path, "dialed_number_is.wav");
                    switch_ivr_play_file(session, NULL, temp_path, NULL);
                    switch_safe_free(temp_path);
                    switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, "SIP callee username:::%s", call_info->callee.cid_name);

                    temp_path = switch_mprintf("en name_spelled pronounced   %s", token_list[atoi(ivr_pin) - 1] /* call_info->callee.cid_name */);
                    switch_core_session_execute_application(session, "say", temp_path);

                    switch_play_and_get_digits(session, 1, 1, 1, 15000, "#", switch_mprintf("%s%s", path, "press_1_2_continue_69.wav"), switch_mprintf("%s%s", path, "no_input.wav"), NULL, digit_buffer,
                                               sizeof(digit_buffer), "[0-1]+", 2000, NULL);
                    ivr_pin = strdup(digit_buffer);
                    switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, "SIP callee number:::%s ivr_pin: %s", call_info->callee.num, ivr_pin);

                    if (atoi(ivr_pin) == 1)
                    {
                        switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, "SIP callee number:::%s", call_info->callee.num);

                        handle_sip_call(channel, dsn, mutex, path, custom_path, call_info);
                        switch_channel_hangup(channel, SWITCH_CAUSE_CALL_REJECTED);
                        // bridge_call(session, call_info, token_list[atoi(ivr_pin)-1] ,dsn,mutex);
                    }
                    else if ((atoi(ivr_pin) == 0))
                    {

                        switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, " GO back previous menu:::%d counter: %zu", atoi(ivr_pin), i);
                    }
                }
                if (atoi(ivr_pin) != 0 && atoi(ivr_pin) > num_tokens)
                {
                    temp_path = switch_mprintf("%s%s", path, "invalid_digit.wav");
                    switch_ivr_play_file(session, NULL, temp_path, NULL);
                    switch_safe_free(temp_path);
                }

                --max_tries;
                switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, " %%%%%%counter: %zu  max_tries:%d   hangup_cause::: %s", i, max_tries, switch_channel_get_variable(channel, "last_bridge_proto_specific_hangup_cause"));

                if (i == 1)
                {
                    switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_DEBUG, " counter: %zu", i);
                    temp_path = switch_mprintf("%s%s", path, "max_tries.wav");
                    switch_ivr_play_file(session, NULL, temp_path, NULL);
                    switch_safe_free(temp_path);
                    switch_channel_hangup(channel, SWITCH_CAUSE_CALL_REJECTED);
                }
            }
        }
        else
        {
            sleep(1);
            temp_path = switch_mprintf("%s%s", path, "feature_69.wav");
            switch_ivr_play_file(session, NULL, temp_path, NULL);
            switch_safe_free(temp_path);
            switch_channel_hangup(channel, SWITCH_CAUSE_CALL_REJECTED);
        }

        switch_safe_free(query);

        break;

    case 70:

        if (call_info->caller.is_dnd == false)
        {
            query = switch_mprintf(" update extension_master SET  dnd =1 where ext_number=%s", caller);
            switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, "Exten query: %s\n", query);
            temp_path = switch_mprintf("%s%s", path, "dnd_activated.wav");
            switch_ivr_play_file(session, NULL, temp_path, NULL);
            switch_safe_free(temp_path);
        }
        else
        {
            query = switch_mprintf(" update extension_master SET  dnd =0 where ext_number=%s", caller);
            switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, "Exten query: %s\n", query);
            temp_path = switch_mprintf("%s%s", path, "dnd_deactivated.wav");
            switch_ivr_play_file(session, NULL, temp_path, NULL);
            switch_safe_free(temp_path);
        }

        execute_sql(dsn, query, mutex);
        switch_safe_free(query);
        break;

    case 72:

        call_frwd = switch_mprintf("SELECT  status FROM `pbx_call_forward` WHERE `customer_id`=%s  AND `extension_id`=%d", call_info->caller.cust_id, call_info->caller.id);
        execute_sql2str(dsn, mutex, call_frwd, status, NELEMS(status));
        switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, "Exten query: %s\n", status);
        if (atoi(status))
        {
            switch_channel_set_variable(channel, "call_type", "call_disable_forward");
            query = switch_mprintf(" update pbx_call_forward SET  status ='0' where customer_id=%s and  extension_id=%d", call_info->caller.cust_id, call_info->caller.id);
            switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, "Exten query: %s\n", status);
            execute_sql(dsn, query, mutex);
            switch_safe_free(query);
            temp_path = switch_mprintf("%s%s", path, "call_forward_deactivated.wav");
            switch_ivr_play_file(session, NULL, temp_path, NULL);
            switch_safe_free(temp_path);
        }
        else if (atoi(status) == 0 && strcmp(status, ""))
        {

            switch_channel_set_variable(channel, "call_type", "call_enable_forward");
            query = switch_mprintf(" update pbx_call_forward SET  status ='1' where customer_id=%s and  extension_id=%d", call_info->caller.cust_id, call_info->caller.id);

            execute_sql(dsn, query, mutex);
            // switch_safe_free(query);
            switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, "Exten query: %s\n", query);
            temp_path = switch_mprintf("%s%s", path, "call_forward_activated.wav");
            switch_ivr_play_file(session, NULL, temp_path, NULL);
            switch_safe_free(temp_path);
        }
        else
        {

            sleep(1);
            temp_path = switch_mprintf("%s%s", path, "call_forward_setup_notfound.wav");
            switch_ivr_play_file(session, NULL, temp_path, NULL);
            switch_safe_free(temp_path);
        }
        break;

    case 75:
        if (call_info->caller.is_vm_on == true)
        {
            voicemail(session, "check", NULL, caller);
        }
        else
        {
            temp_path = switch_mprintf("%s%s", path, "voicemail_is_not_activated.wav");
            switch_ivr_play_file(session, NULL, temp_path, NULL);
            switch_safe_free(temp_path);
        }
        break;
    case 76:
        // switch_channel_set_variable(channel,"ann_pmt",call_info->caller.ann_pmt);
        // if(call_info->caller.is_vm_on == true){
        voicemail(session, "check", NULL, NULL);
        // }else

        break;
    case 80:
    case 81:
        park(session, callee);
        break;

    case 77:
        // if(call_info->caller.is_vm_on == true){

        // ivr(session, min,   max, audio_file,  result);

        switch_play_and_get_digits(session, 2, 10, 3, 15000, "#", audio_file, NULL, NULL, digit_buffer,
                                   sizeof(digit_buffer), "[0-9]+", 2000, NULL);
        mnt = strdup(digit_buffer);
        ivr_pin = strdup(digit_buffer);
        // sscanf(call_info->caller,"%s",atoi(ivr_pin));
        switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, "callee :%s\n", ivr_pin);
        if (strlen(ivr_pin) >= 2)
        {
            if (strlen(ivr_pin) >= 2 && strlen(ivr_pin) <= 4)
            {
                callee = switch_mprintf("%s%s", call_info->caller.cust_id, ivr_pin);
            }
            else
            {
                callee = switch_mprintf("%s", ivr_pin);
            }
            get_ext_details(channel, &call_info->callee, dsn, mutex, callee);

            if (call_info->callee.cust_id != NULL && strcmp(call_info->caller.cust_id, call_info->callee.cust_id))
            {
                sleep(1);
                temp_path = switch_mprintf("%s%s", path, "not_authorized_to_dial.wav");
                switch_ivr_play_file(session, NULL, temp_path, NULL);
                switch_safe_free(temp_path) break;
            }

            if (call_info->callee.is_vm_on == true)
            {

                voicemail(session, NULL, NULL, callee);
            }
            else
            {
                temp_path = switch_mprintf("%s%s", path, "voicemail_is_not_activated.wav");
                switch_ivr_play_file(session, NULL, temp_path, NULL);
                switch_safe_free(temp_path);
            }
        }
        break;

    default:
        sleep(1);
        temp_path = switch_mprintf("%s%s", path, "invalid_digit.wav");
        switch_ivr_play_file(session, NULL, temp_path, NULL);
        switch_safe_free(temp_path);
        break;
    }

    // switch_channel_hangup(channel,SWITCH_CAUSE_CALL_REJECTED);
}

void valetpark(switch_core_session_t *session, const char *callee)
{
    switch_channel_t *channel = switch_core_session_get_channel(session);
    if (atoi(callee + 1) == 5900)
    {
        switch_channel_answer(channel);
        switch_core_session_execute_application(session, "valet_park", "my_lot auto in 5901 5999");
    }

    else if (atoi(callee + 1) >= 5901 && atoi(callee + 1) <= 5999)
    {
        switch_channel_answer(channel);
        switch_core_session_execute_application(session, "valet_park", "my_lot auto out 5901 5999");
    }
}

void park(switch_core_session_t *session, const char *callee)
{
    char *park;
    char *unpark;
    char *data;
    switch_channel_t *channel = switch_core_session_get_channel(session);
    data = switch_mprintf("slot_count=${fifo(count 80@$${domain})");
    switch_core_session_execute_application(session, "set", data);
    switch_core_session_execute_application(session, "set", "slot_count=${slot_count:-9:2}");

    if (atoi(callee + 1) == 80)
    {
        switch_channel_answer(channel);
        switch_core_session_execute_application(session, "unset", "fifo_chime_list");
        switch_core_session_execute_application(session, "set", "fifo_chime_freq=0");
        park = switch_mprintf("80@$${domain} in undef local_stream://moh");
        switch_core_session_execute_application(session, "fifo", park);
    }

    else if (atoi(callee + 1) >= 81 && atoi(callee + 1) <= 99)
    {
        switch_channel_answer(channel);
        unpark = switch_mprintf("80@$${domain} out nowait");
        switch_core_session_execute_application(session, "fifo", unpark);
    }
}

/*-------------------------------------------------------------------------------------------------------*/

void simulate_fake_ring_by_customer_id(switch_channel_t *channel,const char *customer_id, char *custom_path) {

    switch_core_session_t *session = switch_channel_get_session(channel);
        switch_core_session_execute_application(session, "sched_hangup", "+30 allotted_timeout");
        //  switch_core_session_execute_application(session, "set", "ringback=%(2000,4000,440.0,480.0)");
         switch_core_session_execute_application(session, "loop_playback", "/home/cloud/abhishek/fake_tone.wav");
        switch_channel_hangup(channel, SWITCH_CAUSE_CALL_REJECTED);
        
        
       
    
}

/*-------------------------------------------------------------------------------------------------------*/



void ivr(switch_core_session_t *session, uint8_t min, uint8_t max, char *audio_file, const char *result, call_details_t *call_info, char *dsn, switch_mutex_t *mutex, char *path, char *custom_path)
{

    int32_t max_tries = 3;
    uint32_t digit_timeout = 2000;
    int timeout = 15000;
    char digit_buffer[128];
    const char *bad_input_audio_file = NULL;
    const char *var_name = NULL;
    const char *valid_terminators = "#";
    const char *digits_regex = "[0-9]+";
    const char *transfer_on_failure = NULL;

    switch_play_and_get_digits(session, min, max, max_tries, timeout, valid_terminators,
                               audio_file, bad_input_audio_file, var_name, digit_buffer, sizeof(digit_buffer),
                               digits_regex, digit_timeout, transfer_on_failure);
    result = strdup(digit_buffer);
    eaves_drop(session, result, call_info, dsn, mutex,path,custom_path);
}

static int tf_init_callback(void *parg, int argc, char **argv, char **column_names)
{
    tc_failover *tf = (tc_failover *)parg;
    if (argc < 2)
    {
        switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, " Tc_failoverCALLBACK NO DATA %d\n", argc);
        return -1;
    }
    for (int i = 0; i < argc; i++)
        // switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, "INTERNAL tc %d %s %s\n", i, column_names[i], argv[i]);
        tf->is_init = true;
    tf->actve_ftre = atoi(argv[0]);
    tf->actve_val = atoi(argv[1]);

    return 0;
}







void handle_did_dest(switch_channel_t *channel, char *dsn, switch_mutex_t *mutex, char *path, char *custom_path, call_details_t *call)
{

    if (clr_status(channel))
    {

        switch_core_session_t *session = switch_channel_get_session(channel);

        const char *caller = switch_channel_get_variable(channel, "sip_from_user");
        const char *customer_id=call->did.cust_id;
        char *prompt;
        char *time;
        char *invld_pmt;
        char *temp_path, *query = NULL;
        struct stack *pt = newStack(5);
        char result[50] = {'\0'};
        char dst_id[20] = {'\0'};
        char pr_rslt[200] = {'\0'};
        char *did_currnt_call = NULL;
        char did_call[20] = {'\0'};
        char cg_active_id[20] = {'\0'};
        char active_feature[20] = {'\0'};
  

       

        
        // const char *callee = switch_channel_get_variable(channel, "sip_req_user");

        switch_channel_set_variable(channel, "application", "inbound");

        sprintf(dst_id, "%d", call->did.dst_id);
        sprintf(active_feature, "%d", call->did.actv_ftr_id);
        switch_channel_set_variable(channel, "dst_id",dst_id);
        switch_channel_set_variable(channel, "active_feature_id",active_feature);
        // callee= switch_mprintf("%%%s",callee);
        did_currnt_call = switch_mprintf("call verify_did_max_call(%d,'%%%s')", call->did.id, call->did.num);
        execute_sql2str(dsn, mutex, did_currnt_call, did_call, NELEMS(did_call));
        switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, ":%s :: did_call:%s", did_currnt_call, did_call);

        if (atoi(did_call) == 1)
        {
            switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, " call->did.tm_gp_id:%d \t call->did.cust_id:%s", call->did.tm_gp_id, call->did.cust_id);
            time = switch_mprintf("call verify_time_group(%d,%s)", call->did.tm_gp_id, call->did.cust_id);
            execute_sql2str(dsn, mutex, time, result, NELEMS(result));
            switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_DEBUG, "CALL : %s,%s", time, result);
        }

        else
        {
            switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, "-----------DID limit is exhausted-----------");
            switch_channel_set_variable(channel, "custom_hangup", "1007");
            temp_path = switch_mprintf("%s%s", path, "cc_limit_exhaust.wav");
            switch_ivr_play_file(session, NULL, temp_path, NULL);
            switch_safe_free(temp_path);
            switch_channel_hangup(channel, SWITCH_CAUSE_CALL_REJECTED);
            return;
        }

        if (atoi(result) != 1)
        {
            prompt = switch_mprintf("select p.file_path  from pbx_prompts p join  time_group t on(t.prompt_id=p.id) where t.id=%d", call->did.tm_gp_id);
            execute_sql2str(dsn, mutex, prompt, pr_rslt, NELEMS(pr_rslt));

            if (!strcmp(pr_rslt, ""))
            {
                invld_pmt = switch_mprintf("%s%s", path, "time_group_testing_1.wav");
                switch_ivr_play_file(session, NULL, invld_pmt, NULL);
                switch_safe_free(invld_pmt);
            }
            else
            {
                invld_pmt = switch_mprintf("%s%s", custom_path, pr_rslt);
                switch_ivr_play_file(session, NULL, invld_pmt, NULL);
                switch_safe_free(invld_pmt);
            }

            switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, "Time group failover tm_grp_failover:%d  ", call->did.tm_grp_failover);
            if (call->did.tm_grp_failover)
            {
                query = switch_mprintf("SELECT active_feature,active_feature_value FROM `time_group` where id=%d", call->did.tm_gp_id);
                execute_sql_callback(dsn, mutex, query, tf_init_callback, &call->tf);

                sleep(1);
                if (call->tf.actve_ftre != 1)
                {
                    switch_core_session_execute_application(session, "answer", NULL);
                    temp_path = switch_mprintf("%s%s", path, "call_forwarding.wav");
                    switch_ivr_play_file(session, NULL, temp_path, NULL);
                }

                sprintf(cg_active_id, "%d", call->tf.actve_val);
                switch_channel_set_variable(channel, "cg_active_id", cg_active_id);
                switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, " SQL Failover Call Group: %s\n  cg active id::%d cg_active_id:%s", query, call->tf.actve_val, cg_active_id);
                handle_tc_failover(channel, dsn, mutex, path, custom_path, call, call->tc.id, call->tf.actve_ftre, call->tf.actve_val);
            }
            //    switch_channel_hangup(channel, SWITCH_CAUSE_CALL_REJECTED);
            switch_safe_free(prompt);
            return;
        }
        if (call->did.actv_ftr_id >= 2 && call->did.actv_ftr_id <= 21)
        {
            switch_channel_answer(channel);
        }
  
     
        // switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, "----------------------%d",call->did.actv_ftr_id);
        switch (call->did.actv_ftr_id)
        {
        case 0: // feature not active
            switch_channel_set_variable(channel, "custom_hangup", "1018");
            temp_path = switch_mprintf("%s%s", path, "did_feature_not_routed.wav");
            switch_core_session_execute_application(session, "playback", temp_path);
            switch_channel_hangup(channel, SWITCH_CAUSE_CALL_REJECTED);

            break;

        case 1: // SIP
            call->flags.is_inbound_sip = true;
            switch_channel_set_variable_printf(channel, "ext_id", "%s", dst_id);
            sscanf(caller, "%s", call->caller.num);
            call->caller.cust_id = call->did.cust_id;
            switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, "SIP caller:::%s", caller);
            switch_channel_set_variable(channel, "call_type", "sip_extn");
            get_ext_details(channel, &call->callee, dsn, mutex, dst_id);
            handle_sip_call(channel, dsn, mutex, path, custom_path, call);
            break;
        case 2:

            switch_channel_set_variable(channel, "call_type", "call_ivr");
            handle_ivr(channel, dsn, mutex, path, custom_path, call, pt, 1);

            break;

        case 3: // conf
            switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, "active conference : %d", call->did.actv_ftr_id);
            if (verify_internal_exten(channel, dsn, mutex, call, dst_id, path))
            {
                if (call->conf.is_init)
                {
                    handle_conf(channel, call, dsn, mutex, path, custom_path);
                }
            }
            break;
        case 4: // handle queue

            handle_queue(channel, dsn, mutex, path, custom_path, call, 1);
            break;
        case 5:
            if (verify_internal_exten(channel, dsn, mutex, call, dst_id, path))
            {
                if (call->cg.is_init)
                {

                    sscanf(caller, "%s", call->caller.num);
                    call->caller.cust_id = call->did.cust_id;
                    handle_cg(channel, call, dsn, mutex, path, custom_path);
                }
            }

            break;
        case 10:
            // handle tc
            handle_queue(channel, dsn, mutex, path, custom_path, call, 2);
            break;
        case 12:

            handle_play_bck(channel, dsn, mutex, path, custom_path, dst_id);
            break;
        case 13:
            handle_appointment(channel, dsn, mutex, call, path, custom_path);
            break;
        case 16:
             switch_channel_set_variable(channel, "call_type", "call_voicebot");
             query = switch_mprintf("SELECT websocket_url FROM `pbx_voicebot` WHERE  id =%s and sample_rate='1' ", dst_id);
             execute_sql2str(dsn, mutex, query, result, NELEMS(result));
             switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_WARNING, "voicebot url %s dst_id %s ", result,dst_id);
             
             if (strlen(result) != 0)
             {
               switch_core_session_execute_application(session, "conference", switch_mprintf("%s@voicebot_conf+flags{|moderator|unmute|nomoh}",result));

             }
             else
             {
             switch_core_session_execute_application(session, "conference", switch_mprintf("%s@voicebot_conf+flags{|moderator|unmute|nomoh}", switch_channel_get_variable(channel, "call_uuid")));
             }
             
             
            break;
        case 19:
            handle_patch_api(channel, dsn, mutex, call, path, custom_path);
            break;
        case 21:
               
            handle_dynamic_ivr(channel, dsn, mutex, call, path, custom_path);

            break;

        case 22:
              simulate_fake_ring_by_customer_id(channel,customer_id,custom_path);
              switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_WARNING, "Customer id: %s  false ringing play for 30 sec and cdr also\n", call->did.cust_id);
            break;
        default:
            break;
        }
    }

    return;
}



size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t total_size = size * nmemb;
    char **response_buffer = (char **)userp;

    if (*response_buffer == NULL)
    {
        *response_buffer = (char *)malloc(total_size + 1); // Allocate memory for the response buffer (+1 for null terminator)
        if (*response_buffer == NULL)
        {
            fprintf(stderr, "Memory allocation failed for response buffer.\n");
            return 0; // Return 0 to indicate error
        }
    }
    else
    {
        char *temp = (char *)realloc(*response_buffer, total_size + 1); // Resize the buffer to fit new data
        if (temp == NULL)
        {
            fprintf(stderr, "Memory reallocation failed for response buffer.\n");
            return 0; // Return 0 to indicate error
        }
        *response_buffer = temp;
    }

    // Copy the received data to the response buffer
    memcpy(*response_buffer, contents, total_size);
    (*response_buffer)[total_size] = '\0'; // Add null terminator

    return total_size;
}



//abhishek1
size_t write_callback1(void *ptr, size_t size, size_t nmemb, char **response_buffer)
{
    size_t buffer_size = size * nmemb;
    *response_buffer = realloc(*response_buffer, buffer_size + 1);
    if (*response_buffer == NULL) return 0;  // out of memory!
    memcpy(*response_buffer, ptr, buffer_size);
    (*response_buffer)[buffer_size] = '\0';  // null-terminate the string
    return buffer_size;
}

//abhishek2
CURLcode perform_http_request(const char *url, const char *method, const char *data, struct curl_slist *headers, char **response_buffer) {
    CURL *curl = curl_easy_init();
    CURLcode res;
    if (!curl) return CURLE_FAILED_INIT; // initialization failed

    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, method);
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback1);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, response_buffer);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_DEFAULT_PROTOCOL, "https");

    // Set headers if provided
    if (headers)
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    // Set data if provided
    if (data)
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);

    res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    return res;
}





static int dynamic_ivr_init_callback(void *parg, int argc, char **argv, char **column_names)
{
    dynamic_ivr_details_t *dynamic_ivr = (dynamic_ivr_details_t *)parg;

    for (int i = 0; i < argc; i++)
    {
        if (!IS_NULL(argv[0]) && strlen(argv[0]))
        {
            dynamic_ivr->url = strdup(argv[0]);
        }
        if (!IS_NULL(argv[1]) && strlen(argv[1]))
        {
            dynamic_ivr->content_type = strdup(argv[1]);
        }
        if (!IS_NULL(argv[2]) && strlen(argv[2]))
        {
            dynamic_ivr->authorization = strdup(argv[2]);
        }
        if (!IS_NULL(argv[3]) && strlen(argv[3]))
        {
            dynamic_ivr->callback_type = strdup(argv[3]);
        }
    }
    return 0;
}



void handle_dynamic_ivr(switch_channel_t *channel, char *dsn, switch_mutex_t *mutex, call_details_t *call, char *path, char *custom_path)
{
    char digit_buffer[128];
    char digit_select[10];
    switch_core_session_t *session = switch_channel_get_session(channel);
    CURL *curl;
    char *dynamic_data = NULL;
    CURLcode res;
    char *response_buffer = NULL;
    char *file = switch_mprintf("%s%s", path, "press_1_2_3.wav");
    char *playback_file = NULL;
    char dtmffinal[50] = {'\0'};
    char *temp_path = NULL;
    const char *caller = switch_channel_get_variable(channel, "ani");
    //char  destination_url[150] ={'\0'} ;


    
    dynamic_data= switch_mprintf("SELECT destination_url,content_type,authorization,callback_type FROM `dynamic_ivr_details` where id = %d", call->did.dst_id );
    execute_sql_callback(dsn, mutex, dynamic_data, dynamic_ivr_init_callback,&call->dynamic_ivr);
  
    switch_safe_free(dynamic_data)
 

    switch_channel_set_variable(channel, "call_type", "call_dyanmic_ivr");
    switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_CONSOLE, "Customer id------>: %s destination_url %s", call->did.cust_id,call->dynamic_ivr.url);


    if (call->did.cust_id != NULL && !IS_NULL(call->dynamic_ivr.url))
    {
        
        // CURL *curl;
        CURLcode res;
        struct curl_slist *headers = NULL; 
        char *response_buffer = NULL;
        // curl = curl_easy_init();
        const char *data = "{\r\n    \"callback_type\":\"lookup\"\r\n}";
        headers = curl_slist_append(headers, switch_mprintf("Content-Type: %s",call->dynamic_ivr.content_type));
        headers = curl_slist_append(headers, switch_mprintf("Authorization: %s",call->dynamic_ivr.authorization));
        
        res = perform_http_request(call->dynamic_ivr.url, "POST", data, headers, &response_buffer);

            if (res != CURLE_OK)
            {
                fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
            }
            else
            {
                    cJSON *json = cJSON_Parse(response_buffer);
                    size_t data_length = strlen(response_buffer);
                    switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_WARNING, "Received data: %s\n", response_buffer);
                    switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_WARNING, "Length of received data: %zu\n", data_length);
                      if (json)
                       {
                        cJSON *data = cJSON_GetObjectItem(json, "data");
                        if (data) 
                        {
                            cJSON *agents_details = cJSON_GetObjectItem(data, "agents_details");
                            if (agents_details && cJSON_IsArray(agents_details))
                             {
                                int num_agents = cJSON_GetArraySize(agents_details);
                                const char * dial_num =NULL;
                                int answered = 0;
                                for (int i = 0; i < num_agents; i++)
                                 {
                                    cJSON *agent = cJSON_GetArrayItem(agents_details, i);
                                    // int callBridged = 0;
                                    if (agent)
                                     {
                                        cJSON *agent_id = cJSON_GetObjectItem(agent, "agent_id");
                                        cJSON *phone = cJSON_GetObjectItem(agent, "phone");
                                        cJSON *priority = cJSON_GetObjectItem(agent, "priority");
                                        if (agent_id && phone && priority) 
                                        {
                                           dial_num = switch_mprintf("%lld", (long long)phone->valuedouble);

                                            switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_WARNING, "agent_id: %d, dial_num: %s, Priority: %d\n",
                                                              agent_id->valueint,
                                                              dial_num,
                                                              priority->valueint);
                                            bridge_call(session, call, dial_num, dsn, mutex);
                                            
                                           switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_WARNING,"lol %s", switch_channel_get_variable(channel, "DIALSTATUS"));
                                           if (!(strcasecmp(switch_channel_get_variable(channel, "DIALSTATUS"),"SUCCESS")))
                                           {
                                            answered = 1;
                                            break;
                                           }
                                        }
                                    }
                                }
                                if (!answered) 
                                {
                                    switch_ivr_play_file(session, NULL, switch_mprintf("%s%s", path, "ivr-no_user_response.wav"), NULL);
                                }
                            }
                          }
                       }
                   else 
                   {
                    switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, "JSON parsing failed\n");
                    switch_channel_set_variable(channel, "custom_hangup", "1020");
                  }
                  curl_slist_free_all(headers);
                //   curl_easy_cleanup(curl);
            }
        }
        
    
  
    else
    {
        switch_play_and_get_digits(session, 1, 1, 1, 10000, "#", file, NULL, NULL, digit_select,
                                   sizeof(digit_select), "[1-3]+", 2000, NULL);

        if (atoi(digit_select) == 1 || atoi(digit_select) == 0)
        {
            strcpy(digit_select, "1");
            playback_file = switch_mprintf("%s%s", path, "Please_enter_10_digit.wav");
        }
        else if (atoi(digit_select) == 2)
        {
            playback_file = switch_mprintf("%s%s", path, "Please_enter_10_digit_hindi.wav");
        }
        else
        {
            playback_file = switch_mprintf("%s%s", path, "Please_enter_10_digit_marathi.wav");
        }

        for (int i = 0; i < 3; i++)
        {

            switch_play_and_get_digits(session, 1, 11, 1, 20000, "#", playback_file, NULL, NULL, digit_buffer,
                                       sizeof(digit_buffer), "[0-9,*]+", 2000, NULL);

            curl = curl_easy_init();

            if (i == 0)
            {
                strcat(dtmffinal, digit_buffer);
                strcat(dtmffinal, ",");
            }
            else if (i == 1)
            {
                strcat(dtmffinal, digit_buffer);
                strcat(dtmffinal, ",");
            }
            else if (i == 2)
            {
                strcat(dtmffinal, digit_buffer);
            }

            // switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, "digit %d %s %s %s",i,dtmf0,dtmf1,dtmf2 );
            if (curl)
            {
                curl_easy_setopt(curl, CURLOPT_URL, "http://103.139.58.23:8091/api/V1/aov_compalain_register");
                dynamic_data = switch_mprintf("Barcode=%s&Phone_number=%s&IP_Address=&192.158.1.38", digit_buffer, caller);

                // switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_WARNING, "dynamic_data ::  %s /n",dynamic_data );
                curl_easy_setopt(curl, CURLOPT_POSTFIELDS, dynamic_data);
                curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
                curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_buffer);
                res = curl_easy_perform(curl);

                if (res != CURLE_OK)
                {
                    switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
                }
                else
                {
                    // switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_WARNING, "Received data: %s  ", response_buffer);
                    if (response_buffer != NULL)
                    {
                        cJSON *json = cJSON_Parse(response_buffer);
                        if (json)
                        {
                            cJSON *value = cJSON_GetObjectItemCaseSensitive(json, "RESPONSEDATA");
                            if (cJSON_IsString(value) && (value->valuestring != NULL) && !strcasecmp("your complaint has been registered", value->valuestring))
                            {
                                sleep(0.5);
                                if (atoi(digit_select) == 1)
                                {
                                    switch_core_session_execute_application(session, "playback", switch_mprintf("%s%s", path, "Complaint_registered.wav"));
                                }
                                else if (atoi(digit_select) == 2)
                                {
                                    switch_core_session_execute_application(session, "playback", switch_mprintf("%s%s", path, "Complaint_registered_hindi.wav"));
                                }
                                else
                                {
                                    switch_core_session_execute_application(session, "playback", switch_mprintf("%s%s", path, "Complaint_registered_marathi.wav"));
                                }

                                break;
                            }
                            else if (i == 2)
                            {
                                if (atoi(digit_select) == 1)
                                {

                                    temp_path = switch_mprintf("%s%s!%s%s", path, "Max_attempt.wav", path, "Thanks_for_contacting.wav");
                                    switch_channel_set_variable(channel, "playback_delimiter", "!");
                                }
                                else if ((atoi(digit_select) == 2))
                                {
                                    temp_path = switch_mprintf("%s%s!%s%s", path, "Max_attempt_hindi.wav", path, "Thanks_for_contacting_hindi.wav");
                                    switch_channel_set_variable(channel, "playback_delimiter", "!");
                                }
                                else
                                {
                                    temp_path = switch_mprintf("%s%s!%s%s", path, "Max_attempt_marathi.wav", path, "Thanks_for_contacting_marathi.wav");
                                    switch_channel_set_variable(channel, "playback_delimiter", "!");
                                }
                                switch_ivr_play_file(session, NULL, temp_path, NULL);

                                sleep(1);
                                switch_safe_free(temp_path);
                                switch_channel_hangup(channel, SWITCH_CAUSE_CALL_REJECTED);
                                break;
                            }
                            if (strlen(digit_buffer) == 0)
                            {

                                if (atoi(digit_select) == 1)
                                {
                                    playback_file = switch_mprintf("%s%s", path, "no_input.wav");
                                }
                                else if (atoi(digit_select) == 2)
                                {
                                    playback_file = switch_mprintf("%s%s", path, "no_input_hindi.wav");
                                }
                                else
                                {
                                    playback_file = switch_mprintf("%s%s", path, "no_input_marathi.wav");

                                }
                            }
                            else
                            {
                                if (atoi(digit_select) == 1)
                                {
                                    switch_core_session_execute_application(session, "playback", switch_mprintf("%s%s", path, "Try_again.wav"));
                                }
                                else if (atoi(digit_select) == 2)
                                {
                                    switch_core_session_execute_application(session, "playback", switch_mprintf("%s%s", path, "Try_again_hindi.wav"));
                                }
                                else
                                {
                                    switch_core_session_execute_application(session, "playback", switch_mprintf("%s%s", path, "Try_again_marathi.wav"));
                                }
                            }

                            cJSON_Delete(json);
                        }
                        else
                        {
                            switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, "JSON parsing failed\n");
                            switch_channel_set_variable(channel, "custom_hangup", "1020");
                            switch_core_session_execute_application(session, "playback", switch_mprintf("%s%s", path, "custom_api.wav"));
                            break;
                        }
                    }
                }

                curl_easy_cleanup(curl);
            }
        }
        switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, "FINAL DTMF: %s\n  ", dtmffinal);
        switch_channel_set_variable(channel, "dtmf", dtmffinal);
    }

    if (response_buffer)
    {
        free(response_buffer);
        response_buffer = NULL;
    }
}



static int patch_call_init_callback(void *parg, int argc, char **argv, char **column_names)
{
    patch_call_details_t *patch = (patch_call_details_t *)parg;

    for (int i = 0; i < argc; i++)
    {
        if (!IS_NULL(argv[0]) && strlen(argv[0]))
        {
            patch->calling_num = strdup(argv[0]);
        }
        if (!IS_NULL(argv[1]) && strlen(argv[1]))
        {
            patch->custom_uuid = strdup(argv[1]);
        }
    }
    return 0;
}

void handle_patch_api(switch_channel_t *channel, char *dsn, switch_mutex_t *mutex, call_details_t *call, char *path, char *custom_path)
{
    char *src;
    char *sql;
    char digit_buffer[128];
    switch_core_session_t *session = switch_channel_get_session(channel);
    const char *caller = switch_channel_get_variable(channel, "ani");

    sql = switch_mprintf("call switch_patch_api(%s,%s)", caller, call->did.num);
    execute_sql_callback(dsn, mutex, sql, patch_call_init_callback, &call->patch);
    src = call->patch.calling_num;
    switch_channel_set_variable(channel, "custom_uuid", call->patch.custom_uuid);

    switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, "PATCH API calling %s %s caller %s ", src, call->patch.custom_uuid, caller);

    if (!IS_NULL(src))
    {
        switch_channel_set_variable(channel, "user_type", "Customer");
        switch_play_and_get_digits(session, 1, 1, 3, 15000, "#", switch_mprintf("%s%s", path, "patch_api.wav"), NULL, NULL, digit_buffer,
                                   sizeof(digit_buffer), "[1-2]+", 2000, NULL);
        switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, "digit %s", digit_buffer);
        if (!strcmp(digit_buffer, "1"))
        {
            call->flags.is_call_outbound = true;
            switch_channel_set_variable(channel, "call_type", "patch_api");
            switch_channel_set_variable(channel, "application", "outbound");
            switch_channel_set_variable(channel, "cust_id", call->did.cust_id);
            dialoutrule(channel, dsn, mutex, path, custom_path, call, src);
            outbound(session, dsn, mutex, path, custom_path, call, src);
            (call->obd.is_init == 0) ? call->obd.is_init = call->mnt.is_init : call->obd.is_init;
            if (call->obd.is_init)
            {

                bridge_call(session, call, src, dsn, mutex);
            }
        }
        else
        {
            sleep(1);
            switch_core_session_execute_application(session, "playback", switch_mprintf("%s%s", path, "Thanks_for_contacting.wav"));
            switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, "ANY current value for");
        }
    }
    else
    {

        sleep(1);
        switch_core_session_execute_application(session, "playback", switch_mprintf("%s%s", path, "Thanks_for_contacting.wav"));
        switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, "ANY current value for");
        switch_channel_hangup(channel, SWITCH_CAUSE_CALL_REJECTED);
    }
}

//  appointment method
static int apmt_init_callback(void *parg, int argc, char **argv, char **column_names)
{
    apmt_details_t *apmt = (apmt_details_t *)parg;
    if (argc < 22)
    {
        switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, " Appointment  CALLBACK ERROR : NO DATA %d\n", argc);
        return -1;
    }
    for (int i = 0; i < argc; i++)
        // switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, " Appointment %d %s %s\n", i, column_names[i], argv[i]);

        if (!IS_NULL(argv[0]) && strlen(argv[0]))
        {
            apmt->apmt_id = strdup(argv[0]);
        }
    if (!IS_NULL(argv[1]) && strlen(argv[1]))
    {
        apmt->apmt_name = strdup(argv[1]);
    }
    if (!IS_NULL(argv[2]) && strlen(argv[2]))
    {
        apmt->open_time = strdup(argv[2]);
    }
    if (!IS_NULL(argv[3]) && strlen(argv[3]))
    {
        apmt->close_time = strdup(argv[3]);
    }
    if (!IS_NULL(argv[4]) && strlen(argv[4]))
    {
        apmt->tm_bk_srt = strdup(argv[4]);
    }
    if (!IS_NULL(argv[5]) && strlen(argv[5]))
    {
        apmt->wc_pmt_path = strdup(argv[5]);
    }

    if (strlen(argv[6]))
    {
        apmt->invalid_pmt_path = strdup(argv[6]);
    }

    if (strlen(argv[7]))
    {
        apmt->tm_pmt_path = strdup(argv[7]);
    }

    if (!IS_NULL(argv[8]) && strlen(argv[8]))
    {
        strcpy(apmt->phone_no1, argv[8]);
    }
    if (!IS_NULL(argv[9]) && strlen(argv[9]))
    {
        apmt->phone_no2 = strdup(argv[9]);
    }
    if (!IS_NULL(argv[10]) && strlen(argv[10]))
    {
        apmt->tm_bk_end = strdup(argv[10]);
    }
    if (!IS_NULL(argv[11]) && strlen(argv[11]))
    {
        apmt->name = strdup(argv[11]);
    }
    apmt->contact_id = atoi(argv[12]);
    apmt->grp_contact_id = atoi(argv[13]);

    if (!IS_NULL(argv[14]) && strlen(argv[14]))
    {
        apmt->ext_no = strdup(argv[14]);
    }
    apmt->dig_tm_out = atoi(argv[15]);
    apmt->intm_dig_out = atoi(argv[16]);
    apmt->mx_tm_out_try = atoi(argv[17]);
    apmt->mx_invalid_try = atoi(argv[18]);

    if (!IS_NULL(argv[19]) && strlen(argv[19]))
    {
        strcpy(apmt->tm_interval, argv[19]);
    }
    apmt->is_ext = atoi(argv[20]);
    apmt->is_pstn = atoi(argv[21]);

    return 0;
}

static int apmt_slots_init_callback(void *parg, int argc, char **argv, char **column_names)
{
    apmt_slots_details_t *apmt_slots = (apmt_slots_details_t *)parg;
    if (argc < 7)
    {
        switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, " Apmt_slots_init_callback %d\n", argc);
        return -1;
    }

    for (int i = 0; i < argc; i++)
        // switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, " appointment slots %d %s %s\n", i, column_names[i], argv[i]);

        if (!IS_NULL(argv[0]) && strlen(argv[0]))
        {
            apmt_slots->id = strdup(argv[0]);
        }
    if (!IS_NULL(argv[1]) && strlen(argv[1]))
    {
        apmt_slots->app_id = atoi(argv[1]);
    }
    if (!IS_NULL(argv[2]) && strlen(argv[2]))
    {
        apmt_slots->did = strdup(argv[2]);
    }
    if (!IS_NULL(argv[3]) && strlen(argv[3]))
    {
        apmt_slots->src = strdup(argv[3]);
    }
    if (!IS_NULL(argv[4]) && strlen(argv[4]))
    {
        strcpy(apmt_slots->date_slot, argv[4]);
    }
    if (!IS_NULL(argv[5]) && strlen(argv[5]))
    {
        strcpy(apmt_slots->time_start, argv[5]);
    }
    if (!IS_NULL(argv[6]) && strlen(argv[6]))
    {
        strcpy(apmt_slots->time_end, argv[6]);
    }

    apmt_slots->is_init = true;
    return 0;
}

int db_time_apmt(call_details_t *call, char *break_time)
{
    char res_tm_db[50] = {'\0'}; // db time
    struct tm info;
    int hr_db, mnt_db, sec_db;
    int time_db[10];
    int mk_db;

    // database max time
    sscanf(break_time, "%d:%d:%d", &hr_db, &mnt_db, &sec_db);
    info.tm_hour = hr_db;
    info.tm_min = mnt_db;
    mk_db = mktime(&info);
    if (mk_db == -1)
    {
        switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, "Error: unable to make time using mktime\n");
    }
    else
    {
        strftime(res_tm_db, sizeof(res_tm_db), "%H%M", &info);
    }
    sscanf(res_tm_db, "%d", &time_db[0]);
    // switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, " break time start : %d", time_db[0]);
    return time_db[0];
}

int cust_ivr(switch_core_session_t *session, int min, int max, int max_tries, int digit_timeout, char *file, char *invalid_file, char *tm_out_pmt, int timeout, int dig_1, int dig_2, char *path)
{
    char digit_buffer[128], *temp_path;
    const char *result = NULL;
    const char *digits_regex = "[0-9]+";
    const char *transfer_on_failure = NULL;

    int i = 0;

    while (i < max_tries)
    {
        switch_play_and_get_digits(session, min, max, 1, (digit_timeout) * 1000, "#", file, tm_out_pmt, NULL, digit_buffer,
                                   sizeof(digit_buffer), digits_regex, timeout, transfer_on_failure);
        result = strdup(digit_buffer);

        if (atoi(result) >= dig_1 && atoi(result) <= dig_2)
        {
            switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, "Result :%d\n", atoi(result));
            return atoi(result);
        }

        else if (!(atoi(result) >= dig_1 && atoi(result) <= dig_2) && strlen(result) >= 1)
        {
            switch_ivr_play_file(session, NULL, invalid_file, NULL);
            switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, "Invalid digit:%zu &  Result :%s \n", strlen(result), result);
        }

        if (i == (max_tries - 1))
        {
            temp_path = switch_mprintf("%s%s", path, "max_tries.wav");
            switch_ivr_play_file(session, NULL, temp_path, NULL);
            switch_safe_free(temp_path);
        }
        i++;
    }
    return 0;
}

void handle_default_prompts(call_details_t *call, char *wel_pmt, char *invalid_pmt, char *time_out_pmt, char *path, char *custom_path)
{
    
     if (file_exists(switch_mprintf("%s%s", custom_path, wel_pmt)))
     {
         call->dp.wel_pmt = switch_mprintf("%s%s", custom_path, wel_pmt);
     }
     else
     {
         call->dp.wel_pmt = switch_mprintf("%s%s", path, "wel_ectl.wav");
     }


     if (file_exists(switch_mprintf("%s%s", custom_path, invalid_pmt)))
     {
         call->dp.invalid_pmt = switch_mprintf("%s%s", custom_path, invalid_pmt);
     }
     else
     {
         call->dp.invalid_pmt = switch_mprintf("%s%s", path, "invalid_digit.wav");
     }
    
     if (file_exists(switch_mprintf("%s%s", custom_path, time_out_pmt)))
     {
         call->dp.time_out_pmt = switch_mprintf("%s%s", custom_path, time_out_pmt);
     }
     else
     {
         call->dp.time_out_pmt = switch_mprintf("%s%s", path, "max_tries.wav");
     }

    // switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_WARNING, " wel %s  invalid %s tm %s ", call->dp.wel_pmt,call->dp.invalid_pmt,call->dp.time_out_pmt);
}

void time_apmt(switch_channel_t *channel, char *dsn, switch_mutex_t *mutex, call_details_t *call, int open_time, int close_time, int bk_day, char *path, char *custom_path)
{
    char *insert_str = NULL;
    const char *caller = switch_channel_get_variable(channel, "sip_from_user");
    switch_core_session_t *session = switch_channel_get_session(channel);
    char *db_string_tm = NULL, *temp_path = NULL;
    int tm_hr_db, tm_hr, counter, tm_final;
    char *tm_mnt = "00";
    char hr_str[20];
    char res_time[50] = {'\0'};
    char res_tm_sys[50] = {'\0'}; // system time
    int hr_sys, mnt_sys, sec_sys;
    char result_tm[50] = {'\0'};
    int time_sys;
    int tm_adj = 0;
    char *invalid_pmt = NULL;
    char *tm_out_pmt = NULL;
    switch_time_exp_t tm;
    switch_size_t retsize;
    switch_time_t ts;
    ts = switch_time_now();
    switch_time_exp_lt(&tm, ts);
    switch_strftime(res_time, &retsize, sizeof(res_time), "%X", &tm);
    switch_strftime(res_tm_sys, &retsize, sizeof(res_tm_sys), "%H%M", &tm);

    // handle_default_prompts( call,NULL,call->apmt.invalid_pmt_path, call->apmt.tm_pmt_path,path,custom_path);
    invalid_pmt = call->dp.invalid_pmt;
    tm_out_pmt = call->dp.time_out_pmt;

    // time defination & making
    sscanf(res_time, "%d:%d:%d", &hr_sys, &mnt_sys, &sec_sys);
    sscanf(res_tm_sys, "%d", &time_sys);
    // switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, " Time_sys:%d ", time_sys);

    counter = 0;
    tm_hr_db = 0;
    tm_hr = 0;

    if (atoi(call->apmt.tm_interval) == 90)
    {
        tm_adj = 2;
    }
    // switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, " open_time:%d close_time:%d booking_day:%d", open_time,close_time,bk_day);

    for (int i = open_time; tm_hr_db < (close_time - tm_adj); i++)
    {

        sec_sys = 00;

        // 15 mins
        if (atoi(call->apmt.tm_interval) == 15)
        {
            tm_hr_db = open_time;
            tm_mnt=(i % 2 == 1)?"00":"45";
            // if (i % 2 == 1)
            // {
            //     tm_mnt = "00";
            // }
            // else
            // {
            //     tm_mnt = "45";
            // }
            if (i % 2 == 0)
            {
                counter++;
            }
            if (counter % 2 == 1)
            {
                if (i % 2 == 1)
                {
                    tm_mnt = "30";
                }
                else
                {
                    tm_mnt = "15";
                }
            }
            // switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, " Counter: %d,Loop i:%d ", counter, i);
            if (i % 2 == 1 && counter % 2 == 0)
            {
                tm_hr++;
            }
            tm_hr_db = (tm_hr_db) + tm_hr;
        }

        // 30 mins
        if (atoi(call->apmt.tm_interval) == 30)
        {
            tm_hr_db = open_time - 1;
            tm_mnt = "00";
            counter++;

            if (counter % 2)
            {
                tm_hr++;
            }
            if (counter % 2 == 0)
            {
                tm_mnt = "30";
            }
            tm_hr_db = tm_hr_db + tm_hr;
        }

        // 90 mins
        if (atoi(call->apmt.tm_interval) == 90)
        {
            tm_hr_db = open_time - 2;
            tm_mnt = "00";
            counter++;
            if (counter % 2)
            {
                tm_hr += 2;
            }
            if (counter % 2 == 0)
            {
                tm_mnt = "30";
                tm_hr++;
            }
            tm_hr_db = tm_hr_db + tm_hr;
        }

        // 60 min
        if (atoi(call->apmt.tm_interval) == 60 || atoi(call->apmt.tm_interval) == 45)
        {
            tm_hr_db = open_time;
            tm_mnt = "00";
            tm_hr_db = tm_hr_db + counter++;
        }

        if (bk_day == 1)
        {
            db_string_tm = switch_mprintf("SELECT COUNT(1) as count_slot  FROM `pbx_appointment_slots`WHERE app_id = %s AND date_slot = CURRENT_DATE \
    AND time_start = '%d:%s:00' ",
                                          call->apmt.apmt_id, tm_hr_db, tm_mnt);
            execute_sql2str(dsn, mutex, db_string_tm, result_tm, NELEMS(result_tm));
            /* Only uncomment belown statement for debugging mode  */
            // switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, " apmt_count_sql %s \t result_tm:%d \t tm_hr_db:%d \n ", db_string_tm, atoi(result_tm), tm_hr_db);
            switch_safe_free(db_string_tm);
            sprintf(hr_str, "%d", tm_hr_db);
            strcat(hr_str, tm_mnt);
            tm_final = atoi(hr_str);

            if (tm_final >= db_time_apmt(call, call->apmt.tm_bk_srt) && tm_final < db_time_apmt(call, call->apmt.tm_bk_end))
                continue;
            // switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, "time_sys  %d \t Result TIME: %d ", time_sys, tm_final);

            if (atoi(result_tm) == 0 && time_sys < tm_final)
            {
                temp_path = switch_mprintf("%s%s", path, "confirm_slot.wav");
                switch (cust_ivr(session, 1, 1, call->apmt.mx_invalid_try, call->apmt.intm_dig_out, temp_path, invalid_pmt, tm_out_pmt, call->apmt.dig_tm_out, 1, 1, path))

                {
                case 1:
                    insert_str = switch_mprintf("INSERT INTO `cc_master`.`pbx_appointment_slots` (`app_id`, `did`, `src`,\
    `date_slot`, `time_start`, `time_end`) VALUES ('%s', '%s','%s',CURRENT_DATE ,'%d:%d:%d',DATE_ADD( '2021-09-01 %d:%d:%d',INTERVAL '%d' MINUTE))\
    ",
                                                call->apmt.apmt_id, call->did.num, caller, tm_hr_db, atoi(tm_mnt), sec_sys, tm_hr_db, atoi(tm_mnt), sec_sys, atoi(call->apmt.tm_interval));
                    execute_sql(dsn, insert_str, mutex);
                    switch_safe_free(insert_str);
                    temp_path = switch_mprintf("%s%s", path, "apmt_booked.wav");
                    switch_ivr_play_file(session, NULL, temp_path, NULL);
                    // switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, "SLOT IS BOOKED");
                    break;
                }
                return;
            }
        }

        if (bk_day == 2)
        {
            db_string_tm = switch_mprintf("SELECT COUNT(1) as count_slot  FROM `pbx_appointment_slots`WHERE app_id = %s AND  date_slot = DATE_ADD(CURRENT_DATE, INTERVAL 1 DAY) \
    AND time_start = '%d:%s:00' ",
                                          call->apmt.apmt_id, tm_hr_db, tm_mnt);
            execute_sql2str(dsn, mutex, db_string_tm, result_tm, NELEMS(result_tm));
            // switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, " Appointment count_sql %s \t result_tm:%d \t tm_hr_db:%d ", db_string_tm, atoi(result_tm), tm_hr_db);
            switch_safe_free(db_string_tm);
            sprintf(hr_str, "%d", tm_hr_db);
            strcat(hr_str, tm_mnt);
            tm_final = atoi(hr_str);

            if (tm_final >= db_time_apmt(call, call->apmt.tm_bk_srt) && tm_final < db_time_apmt(call, call->apmt.tm_bk_end))
                continue;
            switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, " Result Time .. . . .. . . . %d ", tm_final);

            if (atoi(result_tm) == 0 && time_sys)
            {

                insert_str = switch_mprintf("INSERT INTO `cc_master`.`pbx_appointment_slots` (`app_id`, `did`, `src`,\
    `date_slot`, `time_start`, `time_end`) VALUES ('%s', '%s','%s', DATE_ADD(CURRENT_DATE, INTERVAL 1 DAY) ,'%d:%d:%d',DATE_ADD( '2021-09-01 %d:%d:%d',INTERVAL '%d' MINUTE))\
    ",
                                            call->apmt.apmt_id, call->did.num, caller, tm_hr_db, atoi(tm_mnt), sec_sys, tm_hr_db, atoi(tm_mnt), sec_sys, atoi(call->apmt.tm_interval));
                execute_sql(dsn, insert_str, mutex);
                switch_safe_free(insert_str);
                temp_path = switch_mprintf("%s%s", path, "apmt_booked.wav");
                switch_ivr_play_file(session, NULL, temp_path, NULL);
                switch_channel_set_variable_printf(channel, "date_slot", "%s", call->apmt_slots.date_slot);
                switch_safe_free(temp_path);

                // insert_str = switch_mprintf("en TELEPHONE_NUMBER PRONOUNCED_YEAR MASCULINE %s",call->apmt_slots.date_slot);
                /*  insert_str = switch_mprintf("en short_date_time pronounced [%s]",call->apmt_slots.date_slot);
                switch_core_session_execute_application(session, "say", insert_str); */
                sleep(1);
                return;
            }
        }
        switch_safe_free(tm_out_pmt);
        switch_safe_free(invalid_pmt);
    }
}


void book_appointment(switch_channel_t *channel, char *dsn, switch_mutex_t *mutex, call_details_t *call, int bk_day, char *path, char *custom_path)
{

    int tm_hr_db = 0, tm_hr = 0, tm_adj = 0, tm_final = 0, digit_rcvd = 0, time_sys;
    char *temp_path = NULL, *audio_wel = NULL, *tm_mnt = "00", *db_string_tm = NULL, hr_str[20];
    switch_core_session_t *session = switch_channel_get_session(channel);
    char res_time[50] = {'\0'}, res_tm_sys[50] = {'\0'}, result_tm[50] = {'\0'}, res_date[50]; // system time
    int open_time = 8, close_time = 18, count_mrng = 0, count_aftr = 0, count_eve = 0, hr_sys, mnt_sys, sec_sys, counter;
    #define Morning_Time 9
    #define Afternoon_Time 12
    #define Evening_Time 15
    static int day_counter = 0;

    switch_time_exp_t tm;
    switch_size_t retsize;
    switch_time_t ts;
    ts = switch_time_now();
    switch_time_exp_lt(&tm, ts);
    switch_strftime(res_date, &retsize, sizeof(res_date), "%Y-%m-%d", &tm);
    switch_strftime(res_time, &retsize, sizeof(res_time), "%X", &tm);
    switch_strftime(res_tm_sys, &retsize, sizeof(res_tm_sys), "%H%M", &tm);
    switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, "-----------inside the book appointment----------%d",open_time);

    switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, " handleii  %s  ", call->dp.invalid_pmt);


    // time defination & making

    sscanf(res_time, "%d:%d:%d", &hr_sys, &mnt_sys, &sec_sys);
    sscanf(res_tm_sys, "%d", &time_sys);

    if (atoi(call->apmt.close_time) != 0 && atoi(call->apmt.open_time) != 0)
    {

        open_time = atoi(call->apmt.open_time);
        close_time = atoi(call->apmt.close_time);
    }
    switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, " Time_sys: %d \t Open_time: %d \t Close_time: %d \t Digit_time:%d \b Intermediate_tm_out:%d ", time_sys, open_time, close_time,call->apmt.dig_tm_out, call->apmt.intm_dig_out);

    if (atoi(call->apmt.tm_interval) == 90)
    {
        tm_adj = 2;
    }

    if (bk_day == 1 || bk_day == 2)
    {

        day_counter++;
        // switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, " Day_counter %d ", day_counter);
        if (day_counter == 1)
        {
            day_counter = 0;
            audio_wel = switch_mprintf("%s%s", path, "all_filled.wav");
            switch_ivr_play_file(session, NULL, audio_wel, NULL);
            switch_safe_free(audio_wel);
            // switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, " Day_counter %d ", day_counter);

            return;
        }

        goto case1;
    }
    audio_wel = switch_mprintf("%s%s", path, "book_slot.wav");
    switch_ivr_play_file(session, NULL, audio_wel, NULL);
    switch_safe_free(audio_wel);

    switch (cust_ivr(session, 1, 1, call->apmt.mx_invalid_try, call->apmt.intm_dig_out, audio_wel, call->dp.invalid_pmt, call->dp.time_out_pmt, call->apmt.dig_tm_out, 1, 2, path))
    {

    case1:
    case 1: // for Today slot booking
    {
        counter = 0;
        for (int i = open_time; tm_hr_db < (close_time - tm_adj); i++)
        {
            sec_sys = 00;

            // 15 mins
            if (atoi(call->apmt.tm_interval) == 15)
            {
                tm_hr_db = open_time;
                if (i % 2 == 1)
                {
                    tm_mnt = "00";
                }
                else
                {
                    tm_mnt = "45";
                }

                if (i % 2 == 0)
                {
                    counter++;
                }


                if (counter % 2 == 1)
                {
                    if (i % 2 == 1)
                    {
                        tm_mnt = "30";
                    }
                    else
                    {
                        tm_mnt = "15";
                    }
                }
                // switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, " counter: %d \t i: %d", counter, i);
                if (i % 2 == 1 && counter % 2 == 0)
                {
                    tm_hr++;
                }
                tm_hr_db = (tm_hr_db) + tm_hr;
            }
            // 30 mins
            if (atoi(call->apmt.tm_interval) == 30)
            {
                tm_hr_db = open_time;
                tm_mnt = "00";
                if (i % 2)
                {
                    counter++;
                }

                if (i % 2 == 0)
                {
                    tm_mnt = "30";
                }
                tm_hr_db = tm_hr_db + counter;
            }

            // 90 mins
            if (atoi(call->apmt.tm_interval) == 90)
            {
                tm_hr_db = open_time - 2;
                tm_mnt = "00";
                counter++;
                if (counter % 2)
                {
                    tm_hr += 2;
                }
                if (counter % 2 == 0)
                {
                    tm_mnt = "30";
                    tm_hr++;
                }
                tm_hr_db = tm_hr_db + tm_hr;
            }

            // 60 min
            if (atoi(call->apmt.tm_interval) == 60 || atoi(call->apmt.tm_interval) == 45)
            {
                tm_hr_db = open_time;
                tm_mnt = "00";
                tm_hr_db = tm_hr_db + counter++;
            }

            db_string_tm = switch_mprintf("SELECT COUNT(1) as count_slot  FROM `pbx_appointment_slots`WHERE app_id = %s AND date_slot = CURRENT_DATE \
    AND time_start = '%d:%s:00' ",
                                          call->apmt.apmt_id, tm_hr_db, tm_mnt);
            execute_sql2str(dsn, mutex, db_string_tm, result_tm, NELEMS(result_tm));
            /* Only uncomment belown statement for debugging mode  */
            // switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, " apmt_count_sql: %s \n tm_hr_db: %d \n result_tm: %d \n ", db_string_tm, tm_hr_db, atoi(result_tm));
            switch_safe_free(db_string_tm);
            sprintf(hr_str, "%d", tm_hr_db);
            strcat(hr_str, tm_mnt);
            tm_final = atoi(hr_str);

            if (tm_final >= db_time_apmt(call, call->apmt.tm_bk_srt) && tm_final < db_time_apmt(call, call->apmt.tm_bk_end))
                continue;

           if (atoi(result_tm) == 0 && time_sys < tm_final) {
           count_mrng = (tm_final < 1200) ? 1 : ((tm_final < 1500) ? (count_aftr = 1) : ((tm_final < db_time_apmt(call, call->apmt.close_time)) ? (count_eve = 1) : 0));
           }

        }

        if (time_sys > 0 && time_sys <= 1200)
        {
            if (count_mrng == 1 && count_aftr == 1 && count_eve == 1 )
            {
               digit_rcvd = cust_ivr(session, 1, 1, call->apmt.mx_invalid_try, call->apmt.intm_dig_out, switch_mprintf("%s%s", path, "slot_timing_all.wav"), call->dp.invalid_pmt, call->dp.time_out_pmt, call->apmt.dig_tm_out, 1, 3, path);
                
            if (digit_rcvd >= 1 && digit_rcvd <= 3) {
                time_apmt(channel, dsn, mutex, call,(digit_rcvd == 1) ? open_time :(digit_rcvd == 2) ? Afternoon_Time : Evening_Time,close_time, 1, path, custom_path);
                }
            }
            else if (count_mrng == 1 && count_aftr == 1)
            {
               digit_rcvd = cust_ivr(session, 1, 1, call->apmt.mx_invalid_try, call->apmt.intm_dig_out, switch_mprintf("%s%s", path, "slotma.wav"), call->dp.invalid_pmt, call->dp.time_out_pmt, call->apmt.dig_tm_out, 1, 2, path);
               if (digit_rcvd >= 1 && digit_rcvd <= 2) {
              time_apmt(channel, dsn, mutex, call,(digit_rcvd == 1) ? open_time : Afternoon_Time,close_time, 1, path, custom_path);}
            }

            else if (count_mrng == 1 && count_eve == 1)
            {
               
              digit_rcvd = cust_ivr(session, 1, 1, call->apmt.mx_invalid_try, call->apmt.intm_dig_out,switch_mprintf("%s%s", path, "slotme.wav"), call->dp.invalid_pmt, call->dp.time_out_pmt, call->apmt.dig_tm_out, 1, 2, path);
              if (digit_rcvd >= 1 && digit_rcvd <= 2) {
              time_apmt(channel, dsn, mutex, call,(digit_rcvd == 1) ? open_time : Evening_Time,close_time, 1, path, custom_path);}
            }
            

            else if (count_aftr == 1 && count_eve == 1)
            {
                
            digit_rcvd = cust_ivr(session, 1, 1, call->apmt.mx_invalid_try, call->apmt.intm_dig_out, switch_mprintf("%s%s", path, "slotae.wav"), call->dp.invalid_pmt, call->dp.time_out_pmt, call->apmt.dig_tm_out, 1, 2, path);
            if (digit_rcvd >= 1 && digit_rcvd <= 2) {
              time_apmt(channel, dsn, mutex, call,(digit_rcvd == 1) ? Afternoon_Time : Evening_Time,close_time, 1, path, custom_path);}
            }
               
            
            else if (count_mrng == 1)
            {
                digit_rcvd = cust_ivr(session, 1, 1, call->apmt.mx_invalid_try, call->apmt.intm_dig_out, switch_mprintf("%s%s", path, "slotm.wav"), call->dp.invalid_pmt, call->dp.time_out_pmt, call->apmt.dig_tm_out, 1, 1, path);
                if (digit_rcvd == 1)
                {
                    time_apmt(channel, dsn, mutex, call, open_time, close_time, 1, path, custom_path);
                }
            }

            else if (count_aftr == 1)
            {
                digit_rcvd = cust_ivr(session, 1, 1, call->apmt.mx_invalid_try, call->apmt.intm_dig_out, switch_mprintf("%s%s", path, "slota.wav"), call->dp.invalid_pmt, call->dp.time_out_pmt, call->apmt.dig_tm_out, 1, 1, path);
                if (digit_rcvd == 1)
                {
                    time_apmt(channel, dsn, mutex, call, Afternoon_Time, close_time, 1, path, custom_path);
                }
            }

            else if (count_eve == 1)
            {
                digit_rcvd = cust_ivr(session, 1, 1, call->apmt.mx_invalid_try, call->apmt.intm_dig_out,  switch_mprintf("%s%s", path, "slote.wav"), call->dp.invalid_pmt, call->dp.time_out_pmt, call->apmt.dig_tm_out, 1, 1, path);
                if (digit_rcvd == 1)
                {
                    time_apmt(channel, dsn, mutex, call, Evening_Time, close_time, 1, path, custom_path);
                }
            }

            else
            {
                digit_rcvd = cust_ivr(session, 1, 1, call->apmt.mx_invalid_try, call->apmt.intm_dig_out, switch_mprintf("%s%s", path, "slot_closed.wav"), call->dp.invalid_pmt, call->dp.time_out_pmt, call->apmt.dig_tm_out, 2, 2, path);
                if (digit_rcvd == 2)
                {
                    goto tmr_slot;
                }
            }

            break;
        }

        
        if (time_sys > 1200 && time_sys <= 1500)
        {

           if (count_aftr == 1 && count_eve == 1)
            {
                
            digit_rcvd = cust_ivr(session, 1, 1, call->apmt.mx_invalid_try, call->apmt.intm_dig_out, switch_mprintf("%s%s", path, "slotae.wav"), call->dp.invalid_pmt, call->dp.time_out_pmt, call->apmt.dig_tm_out, 1, 2, path);
            if (digit_rcvd >= 1 && digit_rcvd <= 2) {
              time_apmt(channel, dsn, mutex, call,(digit_rcvd == 1) ? Afternoon_Time : Evening_Time,close_time, 1, path, custom_path);}
            }

            else if (count_aftr == 1)
            {
                digit_rcvd = cust_ivr(session, 1, 1, call->apmt.mx_invalid_try, call->apmt.intm_dig_out, switch_mprintf("%s%s", path, "slota.wav"), call->dp.invalid_pmt, call->dp.time_out_pmt, call->apmt.dig_tm_out, 1, 1, path);
                if (digit_rcvd == 1)
                {
                    time_apmt(channel, dsn, mutex, call, Afternoon_Time, close_time, 1, path, custom_path);
                }
            }

            else if (count_eve == 1)
            {
                digit_rcvd = cust_ivr(session, 1, 1, call->apmt.mx_invalid_try, call->apmt.intm_dig_out, switch_mprintf("%s%s", path, "slote.wav"), call->dp.invalid_pmt, call->dp.time_out_pmt, call->apmt.dig_tm_out, 1, 1, path);
                if (digit_rcvd == 1)
                {
                    time_apmt(channel, dsn, mutex, call, Evening_Time, 1, close_time, path, custom_path);
                }
            }

            else
            {
                digit_rcvd = cust_ivr(session, 1, 1, call->apmt.mx_invalid_try, call->apmt.intm_dig_out, switch_mprintf("%s%s", path, "slot_closed.wav"), call->dp.invalid_pmt, call->dp.time_out_pmt, call->apmt.dig_tm_out, 2, 2, path);
                if (digit_rcvd == 2)
                {
                    goto tmr_slot;
                }
            }
            break;
        }

        if (count_eve == 1 && time_sys >= 1500)
        {
            digit_rcvd = cust_ivr(session, 1, 1, call->apmt.mx_invalid_try, call->apmt.intm_dig_out, switch_mprintf("%s%s", path, "slote.wav"), call->dp.invalid_pmt, call->dp.time_out_pmt, call->apmt.dig_tm_out, 1, 1, path);
            if (digit_rcvd == 1)
            {
                time_apmt(channel, dsn, mutex, call, Evening_Time, close_time, 1, path, custom_path);
            }
            break;
        }

        else
        {
            digit_rcvd = cust_ivr(session, 1, 1, call->apmt.mx_invalid_try, call->apmt.intm_dig_out, switch_mprintf("%s%s", path, "slot_closed.wav"), call->dp.invalid_pmt, call->dp.time_out_pmt, call->apmt.dig_tm_out, 2, 2, path);
            if (digit_rcvd == 2)
            {
                goto tmr_slot;
            }
        }
        break;
    }

        // tmr  Morning time

    Morning:
    {
        temp_path = switch_mprintf("%s%s", path, "confirm_slot.wav");
        switch (cust_ivr(session, 1, 1, call->apmt.mx_invalid_try, call->apmt.intm_dig_out, temp_path, call->dp.invalid_pmt, call->dp.time_out_pmt, call->apmt.dig_tm_out, 1, 1, path))
        {
        case 1:
            // switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, " tmr morining -----------------------------------");

            time_apmt(channel, dsn, mutex, call, open_time, close_time, 2, path, custom_path);
            break;
        }
        break;
        switch_safe_free(temp_path);
    }
    // tmr  Afternoon time
    Afternoon:
    {
        temp_path = switch_mprintf("%s%s", path, "confirm_slot.wav");
        switch (cust_ivr(session, 1, 1, call->apmt.mx_invalid_try, call->apmt.intm_dig_out, temp_path, call->dp.invalid_pmt, call->dp.time_out_pmt, call->apmt.dig_tm_out, 1, 1, path))
        {
        case 1:
            time_apmt(channel, dsn, mutex, call, Afternoon_Time, close_time, 2, path, custom_path);
            break;
        }
        break;
        switch_safe_free(temp_path);
    }
    // tmr  Afternoon time
    Evening:
    {
        temp_path = switch_mprintf("%s%s", path, "confirm_slot.wav");
        switch (cust_ivr(session, 1, 1, call->apmt.mx_invalid_try, call->apmt.intm_dig_out, temp_path, call->dp.invalid_pmt, call->dp.time_out_pmt, call->apmt.dig_tm_out, 1, 1, path))
        {
        case 1:
            time_apmt(channel, dsn, mutex, call, Evening_Time, close_time, 2, path, custom_path);
            break;
        }
        break;
        switch_safe_free(temp_path);
    }
    // checking slot avaliablity for tmr slot
    case 2:
    {

    tmr_slot:

        counter = 0, tm_hr_db = 0, tm_hr = 0;


        for (int i = open_time; tm_hr_db < (close_time - tm_adj); i++)
        {
            sec_sys = 00;
            // 15 mins
            if (atoi(call->apmt.tm_interval) == 15)
            {
                tm_hr_db = open_time;
                if (i % 2 == 1)
                {
                    tm_mnt = "00";
                }
                else
                {
                    tm_mnt = "45";
                }
                if (i % 2 == 0)
                {
                    counter++;
                }
                if (counter % 2 == 1)
                {
                    if (i % 2 == 1)
                    {
                        tm_mnt = "30";
                    }
                    else
                    {
                        tm_mnt = "15";
                    }
                }
                if (i % 2 == 1 && counter % 2 == 0)
                {
                    tm_hr++;
                }
                tm_hr_db = (tm_hr_db) + tm_hr;
            }
            // 30 mins
            if (atoi(call->apmt.tm_interval) == 30)
            {
                tm_hr_db = open_time - 1;
                tm_mnt = "00";
                counter++;
                if (counter % 2)
                {
                    tm_hr++;
                }
                if (counter % 2 == 0)
                {
                    tm_mnt = "30";
                }
                tm_hr_db = tm_hr_db + tm_hr;
                // switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, " Counter %d. i:%d", counter, i);
            }

            // 90 mins
            if (atoi(call->apmt.tm_interval) == 90)
            {
                tm_hr_db = open_time - 2;
                tm_mnt = "00";
                counter++;
                if (counter % 2)
                {
                    tm_hr += 2;
                }
                if (counter % 2 == 0)
                {
                    tm_mnt = "30";
                    tm_hr++;
                }
                tm_hr_db = tm_hr_db + tm_hr;
                // switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, " Counter:%d & i:%d & close_time %d & tm_hr_db:%d", counter, i, close_time, tm_hr_db);
            }

            // 60 min
            if (atoi(call->apmt.tm_interval) == 60 || atoi(call->apmt.tm_interval) == 45)
            {
                tm_hr_db = open_time;
                tm_mnt = "00";
                tm_hr_db = tm_hr_db + counter++;
            }

            db_string_tm = switch_mprintf("SELECT COUNT(1) as count_slot  FROM `pbx_appointment_slots`WHERE app_id = %s AND date_slot = DATE_ADD(CURRENT_DATE, INTERVAL 1 DAY)\
    AND time_start = '%d:%s:00' ",
                                          call->apmt.apmt_id, tm_hr_db, tm_mnt);
            execute_sql2str(dsn, mutex, db_string_tm, result_tm, NELEMS(result_tm));
            /* Only uncomment belown statement for debugging mode  */
            // switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, " Appointment count_sql: %s & tm_hr_db: %d &  Result_tm: %d \n ", db_string_tm, tm_hr_db, atoi(result_tm));
            switch_safe_free(db_string_tm);
            sprintf(hr_str, "%d", tm_hr_db);
            strcat(hr_str, tm_mnt);
            tm_final = atoi(hr_str);

            if (tm_final >= db_time_apmt(call, call->apmt.tm_bk_srt) && tm_final < db_time_apmt(call, call->apmt.tm_bk_end))
                continue;
            if (atoi(result_tm) == 0)
            {
                if (tm_final < 1200)
                {
                    count_mrng = 1;
                }

                if (tm_final >= 1200 && tm_final < 1500)
                {
                    count_aftr = 1;
                }

                if (tm_final >= 1500 && tm_final < db_time_apmt(call, call->apmt.close_time))
                {
                    count_eve = 1;
                }
            }
        }


        if (count_mrng == 1 && count_aftr == 1 && count_eve == 1)
        {
            // cust_ivr
            temp_path = switch_mprintf("%s%s", path, "slot_timing_all.wav");
            digit_rcvd = cust_ivr(session, 1, 1, call->apmt.mx_invalid_try, call->apmt.intm_dig_out, temp_path, call->dp.invalid_pmt, call->dp.time_out_pmt, call->apmt.dig_tm_out, 1, 3, path);
            switch_safe_free(temp_path);
            switch (digit_rcvd)
            {
            case 1:
            {
                goto Morning;
                break;
            }
            case 2:
            {
                goto Afternoon;
                break;
            }
            case 3:
            {
                goto Evening;
                break;
            }
            }
        }

        else if (count_mrng == 1 && count_aftr == 1)
        {
            // ivr morning & afternoon
            temp_path = switch_mprintf("%s%s", path, "slotma.wav");
            digit_rcvd = cust_ivr(session, 1, 1, call->apmt.mx_invalid_try, call->apmt.intm_dig_out, temp_path, call->dp.invalid_pmt, call->dp.time_out_pmt, call->apmt.dig_tm_out, 1, 2, path);
            switch_safe_free(temp_path);
            if (digit_rcvd == 1)
            {
                goto Morning;
            }
            else if (digit_rcvd == 2)
            {
                goto Afternoon;
            }
        }

        else if (count_mrng == 1 && count_eve == 1)
        {
            // ivr morning & Eve
            temp_path = switch_mprintf("%s%s", path, "slotme.wav");
            digit_rcvd = cust_ivr(session, 1, 1, call->apmt.mx_invalid_try, call->apmt.intm_dig_out, temp_path, call->dp.invalid_pmt, call->dp.time_out_pmt, call->apmt.dig_tm_out, 1, 2, path);
            switch_safe_free(temp_path);
            if (digit_rcvd == 1)
            {
                goto Morning;
            }
            else if (digit_rcvd == 2)
            {
                goto Evening;
            }
        }

        else if (count_aftr == 1 && count_eve == 1)
        {
            // ivr afternoon &  eve
            temp_path = switch_mprintf("%s%s", path, "slotae.wav");
            digit_rcvd = cust_ivr(session, 1, 1, call->apmt.mx_invalid_try, call->apmt.intm_dig_out, temp_path, call->dp.invalid_pmt, call->dp.time_out_pmt, call->apmt.dig_tm_out, 1, 2, path);
            switch_safe_free(temp_path);
            if (digit_rcvd == 1)
            {
                goto Afternoon;
            }
            else if (digit_rcvd == 2)
            {
                goto Evening;
            }
        }

        else if (count_mrng == 1)
        {
            // ivr only morning
            temp_path = switch_mprintf("%s%s", path, "slotm.wav");
            digit_rcvd = cust_ivr(session, 1, 1, call->apmt.mx_invalid_try, call->apmt.intm_dig_out, temp_path, call->dp.invalid_pmt, call->dp.time_out_pmt, call->apmt.dig_tm_out, 1, 1, path);
            switch_safe_free(temp_path);
            if (digit_rcvd == 1)
            {
                goto Morning;
            }
        }

        else if (count_aftr == 1)
        {
            // ivr only afternoon
            temp_path = switch_mprintf("%s%s", path, "slota.wav");
            digit_rcvd = cust_ivr(session, 1, 1, call->apmt.mx_invalid_try, call->apmt.intm_dig_out, temp_path, call->dp.invalid_pmt, call->dp.time_out_pmt, call->apmt.dig_tm_out, 1, 1, path);
            switch_safe_free(temp_path);
            if (digit_rcvd == 1)
            {
                goto Afternoon;
            }
        }

        else if (count_eve == 1)
        {
            // ivr only eve
            temp_path = switch_mprintf("%s%s", path, "slote.wav");
            digit_rcvd = cust_ivr(session, 1, 1, call->apmt.mx_invalid_try, call->apmt.intm_dig_out, temp_path, call->dp.invalid_pmt, call->dp.time_out_pmt, call->apmt.dig_tm_out, 1, 1, path);
            switch_safe_free(temp_path);
            if (digit_rcvd == 1)
            {
                goto Evening;
            }
        }

        else
        {

            temp_path = switch_mprintf("%s%s", path, "tmr_bk_today.wav");
            digit_rcvd = cust_ivr(session, 1, 1, call->apmt.mx_invalid_try, call->apmt.intm_dig_out, temp_path, call->dp.invalid_pmt, call->dp.time_out_pmt, call->apmt.dig_tm_out, 1, 1, path);
            if (digit_rcvd == 1)
            { // today_slot
                book_appointment(channel, dsn, mutex, call, 1, path, custom_path);
            }
        }
    }
    }

    // switch_channel_hangup(channel, SWITCH_CAUSE_CALL_REJECTED);
}

bool verify_apmt_slots(switch_channel_t *channel, call_details_t *call, char *dsn, switch_mutex_t *mutex, char *path, char *custom_path)
{
    char *sql = NULL;
    char *time_sql = NULL;
    char digit_buffer[128];
    const char *result_id = NULL;
    char *invalid_pmt = NULL, *temp_path = NULL, *tm_out_pmt = NULL;

    switch_core_session_t *session = switch_channel_get_session(channel);
    const char *caller = switch_channel_get_variable(channel, "sip_from_user");

    tm_out_pmt = call->apmt.tm_pmt_path;
    invalid_pmt = call->apmt.invalid_pmt_path;

    time_sql = switch_mprintf("SELECT `id`, `app_id`, `did`, `src`, `date_slot`, `time_start`, `time_end` FROM   `pbx_appointment_slots`\
    pbx_appointment    WHERE  date_slot = CURRENT_DATE AND src = %s AND app_id = %s AND CURRENT_TIME BETWEEN  time_start AND time_end  limit 1",
                              caller, call->apmt.apmt_id);

    execute_sql_callback(dsn, mutex, time_sql, apmt_slots_init_callback, &call->apmt_slots);
    // switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, "Caller number :%s and apmt_slot_number :%s  \t  time sql  :%s\n ", call->apmt_slots.src, call->apmt.apmt_id,time_sql);
    switch_safe_free(time_sql);

    if (call->apmt_slots.src == NULL)
    {
        sql = switch_mprintf("SELECT `id` , `app_id` , `did` ,`src`, `date_slot` , `time_start`, `time_end` FROM pbx_appointment_slots\
    WHERE src = %s  AND app_id = %s AND CURRENT_TIME < time_end  AND date_slot = CURRENT_DATE or date_slot = DATE_ADD(CURRENT_DATE, INTERVAL 1 DAY)\
    AND  app_id = %s AND src = %s ",caller, call->apmt.apmt_id, call->apmt.apmt_id, caller);
        // switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, " sql count query: %s  \n Caller number present count:- %d  \n", sql, call->apmt_slots.is_init);
        execute_sql_callback(dsn, mutex, sql, apmt_slots_init_callback, &call->apmt_slots);
        switch_safe_free(sql);

        if (call->apmt_slots.is_init == true)
        {
            temp_path = switch_mprintf("%s%s", path, "apmt_del_1_2.wav");
            switch_safe_free(temp_path);
            switch (cust_ivr(session, 1, 1, call->apmt.mx_invalid_try, call->apmt.intm_dig_out, temp_path, invalid_pmt, tm_out_pmt, call->apmt.dig_tm_out, 1, 2, path))
            {
            case 1:
                book_appointment(channel, dsn, mutex, call, 0, path, custom_path);
                break;


            case 2:
                for (int i = 0; i < call->apmt.mx_invalid_try; i++)
                {
                    // switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, "ApmtID Size :%zu\n", strlen(call->apmt_slots.id));
                    temp_path = switch_mprintf("%s%s", path, "del_id_apmt.wav");
                    switch_play_and_get_digits(session, strlen(call->apmt_slots.id), strlen(call->apmt_slots.id), 1, (call->apmt.mx_tm_out_try) * 1000, "#", temp_path,
                                               tm_out_pmt, NULL, digit_buffer, sizeof(digit_buffer), "[0-9]+", (call->apmt.dig_tm_out) * 1000, NULL);
                    switch_safe_free(temp_path);
                    result_id = strdup(digit_buffer);

                    if (strlen(digit_buffer) > 0)
                    {

                        if (strlen(digit_buffer) == 0)
                        {
                            switch_ivr_play_file(session, NULL, tm_out_pmt, NULL);
                            return 0;
                        }

                        sql = switch_mprintf("SELECT `id` , `app_id` , `did` ,`src`, `date_slot` , `time_start`, `time_end` FROM pbx_appointment_slots WHERE id = %s ", result_id);
                        execute_sql_callback(dsn, mutex, sql, apmt_slots_init_callback, &call->apmt_slots);
                        switch_safe_free(sql);
                        // switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, " Result_id: %s \t  %d \n", result_id, strcmp(caller, call->apmt_slots.src));

                        if (atoi(result_id) == atoi(call->apmt_slots.id) && strcmp(caller, call->apmt_slots.src) == 0 && atoi(call->apmt.apmt_id) == call->apmt_slots.app_id)
                        {
                            sql = switch_mprintf("DELETE FROM pbx_appointment_slots WHERE `id` = %s AND `src`= %s LIMIT 1 ", call->apmt_slots.id, caller);
                            switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, " Apmt delete sql : %s \n", sql);
                            execute_sql_callback(dsn, mutex, sql, apmt_slots_init_callback, &call->apmt_slots);
                            switch_safe_free(sql);

                            temp_path = switch_mprintf("%s%s", path, "del_suc_fly.wav");
                            switch_ivr_play_file(session, NULL, temp_path, NULL);
                            switch_channel_hangup(channel, SWITCH_CAUSE_CALL_REJECTED);
                            return 0;
                        }
                        temp_path = switch_mprintf("%s%s", path, "apmt_no_del.wav");
                        switch_ivr_play_file(session, NULL, temp_path, NULL);
                        switch_safe_free(temp_path);
                    }
                    if (i == 2)
                    {
                        switch_ivr_play_file(session, NULL, tm_out_pmt, NULL);
                        return false;
                    }
                }
                break;
            
            }
        }
        else
        {
            book_appointment(channel, dsn, mutex, call, 0, path, custom_path);
        }
        switch_channel_hangup(channel, SWITCH_CAUSE_CALL_REJECTED);
        return false;
    }

    else
    {
        // switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, "Calling Appointment  %s", call->apmt_slots.src);
        return true;
    }

    switch_safe_free(tm_out_pmt);
    switch_safe_free(invalid_pmt);
}

// appointment handle

void handle_appointment(switch_channel_t *channel, char *dsn, switch_mutex_t *mutex, call_details_t *call, char *path, char *custom_path)
{
    char *wel_pmt = NULL, *sql = NULL;
    switch_core_session_t *session = switch_channel_get_session(channel);
    const char *caller = switch_channel_get_variable(channel, "sip_from_user");
    const char *callee = switch_channel_get_variable(channel, "sip_req_user");

    char *token = NULL, *rest = NULL;

    rest = call->apmt.phone_no1;
    if (call->did.is_init == true)
    {

        switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, " CALLER: %s  AND  DID: %s \n", caller, callee);
        sql = switch_mprintf("call switch_appointment_handle(%d)", call->did.dst_id);

        execute_sql_callback(dsn, mutex, sql, apmt_init_callback, &call->apmt);
        switch_safe_free(sql);
        switch_channel_set_variable(channel, "call_type", "call_appointment");
        switch_channel_set_variable(channel, "ref_id", call->apmt.apmt_id);
        switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, " appointment ref_id :%s  ", call->apmt.apmt_id);

        // welcoming ivr
        sleep(1);

        handle_default_prompts(call, call->apmt.wc_pmt_path, call->apmt.invalid_pmt_path, call->apmt.tm_pmt_path, path, custom_path);
        wel_pmt = call->dp.wel_pmt;
        switch_core_session_execute_application(session, "playback", wel_pmt);
        switch_safe_free(wel_pmt);

        if (verify_apmt_slots(channel, call, dsn, mutex, path, custom_path) && (!IS_NULL(call->apmt_slots.src)))
        {
            
            switch_ivr_play_file(session, NULL, switch_mprintf("%s%s", path, "apmt_call.wav"), NULL);
            
            if ((call->apmt.is_ext == true))
            {   
               switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, "-----------inside 123456 the verify condition---abhishek yadav-----------%d",call->did.actv_ftr_id);

                bridge_call(session, call, call->apmt.ext_no, dsn, mutex);
            }

            if (call->apmt.is_pstn == true && (call->did.is_outbound_on == true))
            {
                               switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, "-----------inside 00000000 the verify condition---abhishek yadav-----------%d",call->did.actv_ftr_id);

                call->caller.cust_id = call->did.cust_id;
                // switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, " PSTN Number:%s\n", call->apmt.phone_no1);
                token = strtok_r(call->apmt.phone_no1, ",", &rest);
                switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, "Number---- %s \n", token);
                while (token != NULL)
                {
                                   switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, "-----------while 123456 the verify condition---abhishek yadav-----------%d",call->did.actv_ftr_id);

                    if (dialoutrule(channel, dsn, mutex, path, custom_path, call, token) &&
                        outbound(session, dsn, mutex, path, custom_path, call, token) &&
                        (call->obd.is_init || call->flags.is_outbound_mnt || call->flags.is_outbound_grp_mnt))
                    {
                        call->flags.is_call_outbound = true;
                        bridge_call(session, call, token, dsn, mutex);
                    }
                    // switch_channel_hangup(channel, SWITCH_CAUSE_CALL_REJECTED);
                    token = strtok_r(NULL, ",", &rest);
                }
                
            }
            
        }

    
    }
}


void handle_tc_failover(switch_channel_t *channel, char *dsn, switch_mutex_t *mutex, char *path, char *custom_path, call_details_t *call, char *tc_id, int actve_ftre, int actve_val)
{
    if (clr_status(channel))
    {

        switch_core_session_t *session = switch_channel_get_session(channel);
        // char *query = NULL;
        char *temp_path = NULL;
        char *query = NULL;
        const char *caller = switch_channel_get_variable(channel, "sip_from_user");
        struct stack *pt = newStack(5);
        char dst_id[20] = {'\0'};
        char result[20] = {'\0'};
        switch_channel_set_variable(channel, "application", "inbound");
        switch_core_session_execute_application(session, "answer", NULL);
        switch_channel_set_variable(channel, "did_id", dst_id);
        switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, "Dial_string : %s %s %s  actve_ftre:%d actve_val:%d", call->caller.cust_id, dst_id, caller, actve_ftre, actve_val);

        call->did.dst_id = actve_val;
        sprintf(dst_id, "%d", actve_val);

        if (actve_ftre != 0 && actve_val != 0)
        {
            switch (actve_ftre)
            {
            case 1: // SIP
                call->flags.is_inbound_sip = true;
                switch_channel_set_variable_printf(channel, "ext_id", "%s", dst_id);
                sscanf(caller, "%s", call->caller.num);
                call->caller.cust_id = call->did.cust_id;
                switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, "Dial_string : cust_id :: %s dst_id :: %s caller:: %sactve_val:: %d", call->caller.cust_id, dst_id, caller, actve_val);
                get_ext_details(channel, &call->callee, dsn, mutex, dst_id);
                handle_sip_call(channel, dsn, mutex, path, custom_path, call);
                break;
            case 2:

                switch_channel_set_variable(channel, "call_type", "call_ivr");
                handle_ivr(channel, dsn, mutex, path, custom_path, call, pt, 1);

                break;

            case 3:
                if (verify_internal_exten(channel, dsn, mutex, call, dst_id, path))
                {
                    if (call->conf.is_init)
                    {
                        handle_conf(channel, call, dsn, mutex, path, custom_path);
                    }
                }
                break;
            case 4:
                handle_queue(channel, dsn, mutex, path, custom_path, call, 1);
                break;
            case 5:
                if (verify_internal_exten(channel, dsn, mutex, call, dst_id, path))
                {
                    if (call->cg.is_init)
                    {

                        sscanf(caller, "%s", call->caller.num);
                        call->caller.cust_id = call->did.cust_id;
                        handle_cg(channel, call, dsn, mutex, path, custom_path);
                    }
                }

                break;
            case 6:
                query = switch_mprintf("select ext_number from  extension_master where  id =%s", dst_id);

                execute_sql2str(dsn, mutex, query, result, NELEMS(result));

                if (get_ext_details(channel, &call->caller, dsn, mutex, result))
                {
                    if (call->caller.is_vm_on)
                    {
                        voicemail(session, NULL, NULL, result);
                    }
                    else
                    {
                        sleep(1);
                        temp_path = switch_mprintf("%s%s", path, "forward_vm_blocked.wav");
                        switch_ivr_play_file(session, NULL, temp_path, NULL);
                        switch_safe_free(temp_path);
                    }
                    switch_safe_free(query);
                }
                break;
            case 8:
                if (call->did.is_outbound_on == true)
                {
                    call->flags.is_call_outbound = true;
                    call->caller.cust_id = call->did.cust_id;

                    query = switch_mprintf("SELECT pbx_contact_list.phone_number1 FROM `pbx_contact_list` WHERE id = %d", actve_val);
                    execute_sql2str(dsn, mutex, query, result, NELEMS(result));
                    switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_DEBUG, " pbx_contact_list id %d Number:%s\n", actve_val, result);
                    if (dialoutrule(channel, dsn, mutex, path, custom_path, call, result) && outbound(session, dsn, mutex, path, custom_path, call, result) && (call->obd.is_init || call->flags.is_outbound_mnt || call->flags.is_outbound_grp_mnt))
                    {
                        bridge_call(session, call, result, dsn, mutex);
                    }
                    switch_safe_free(query);
                }
                break;

            case 10:
                handle_queue(channel, dsn, mutex, path, custom_path, call, 2);
                break;
            case 12:

                handle_play_bck(channel, dsn, mutex, path, custom_path, dst_id);
                break;

            default:
                break;
            }
        }
    }
    return;
}

static int cq_init_callback(void *parg, int argc, char **argv, char **column_names)
{
    queue_details_t *cq = (queue_details_t *)parg;
    if (argc < 11)
    {
        switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, " queue_details CALLBACK DATA %d\n", argc);
        return -1;
    }
    for (int i = 0; i < argc; i++)
        // switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, "Call QUEUE: %d %s %s\n", i, column_names[i], argv[i]);

        cq->is_init = true;
    if (!IS_NULL(argv[0]) && strlen(argv[0]))
    {
        strcpy(cq->q_name, argv[0]);
    }
    cq->max_wait_call = atoi(argv[1]);
    cq->p_anc = atoi(argv[2]);
    cq->p_anc_time = atoi(argv[3]);
    cq->play_pos_on_call = atoi(argv[4]);
    cq->play_pos_prdcly = atoi(argv[5]);
    if (!IS_NULL(argv[6]) && strlen(argv[6]))
    {
        cq->welcome_pmt = strdup(argv[6]);
    }
    if (!IS_NULL(argv[7]) && strlen(argv[7]))
    {
        cq->ann_pmt = strdup(argv[7]);
    }

    cq->stcky_agnt = atoi(argv[8]);

    if (!IS_NULL(argv[9]) && strlen(argv[9]))
    {
        cq->unauth_usr = strdup(argv[9]);
    }

    cq->recording = atoi(argv[10]);
    return 0;
}

static int un_tc_init_callback(void *parg, int argc, char **argv, char **column_names)
{
    unauth_tc_details_t *un_tc = (unauth_tc_details_t *)parg;

    if (argc < 12)
    {
        switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, " un_tc_init_callback NO DATA %d\n", argc);
        return -1;
    }

    for (int i = 0; i < argc; i++)
        // switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, "INTERNAL UNathroized tc %d %s %s\n", i, column_names[i], argv[i]);

        un_tc->is_init = true;
    if (!IS_NULL(argv[0]) && strlen(argv[0]))
    {
        strcpy(un_tc->tc_name, argv[0]);
    }

    if (!IS_NULL(argv[1]) && strlen(argv[1]))
    {
        un_tc->id = strdup(argv[1]);
    }
    un_tc->mnt = atoi(argv[2]);
    un_tc->max_wait_call = atoi(argv[3]);
    if (!IS_NULL(argv[4]) && strlen(argv[4]))
    {
        un_tc->welcome_pmt = strdup(argv[4]);
    }
    if (!IS_NULL(argv[5]) && strlen(argv[5]))
    {
        un_tc->ann_pmt = strdup(argv[5]);
    }
    un_tc->p_anc = atoi(argv[6]);
    un_tc->p_anc_time = atoi(argv[7]);
    un_tc->play_pos_on_call = atoi(argv[8]);
    un_tc->play_pos_prdcly = atoi(argv[9]);
    un_tc->unauth_failover = atoi(argv[10]);
    un_tc->tc_caller_id = atoi(argv[11]);

    return 0;
}

static int tc_init_callback(void *parg, int argc, char **argv, char **column_names)
{
    tc_details_t *tc = (tc_details_t *)parg;
    if (argc < 15)
    {
        switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, " Tc_details CALLBACK NO DATA %d\n", argc);
        return -1;
    }
    for (int i = 0; i < argc; i++)
        // switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, "INTERNAL tc %d %s %s\n", i, column_names[i], argv[i]);

        tc->is_init = true;
    if (!IS_NULL(argv[0]) && strlen(argv[0]))
    {
        strcpy(tc->tc_name, argv[0]);
    }
    if (!IS_NULL(argv[1]) && strlen(argv[1]))
    {
        tc->id = strdup(argv[1]);
    }

    if (!IS_NULL(argv[2]) && strlen(argv[2]))
    {
        tc->cntct1 = strdup(argv[2]);
    }
    if (!IS_NULL(argv[3]) && strlen(argv[3]))
    {
        tc->cntct2 = strdup(argv[3]);
    }

    tc->mnt = atoi(argv[4]);
    if (!IS_NULL(argv[5]) && strlen(argv[5]))
    {
        tc->mnt_id = strdup(argv[5]);
    }

    tc->max_wait_call = atoi(argv[6]);
    if (!IS_NULL(argv[7]) && strlen(argv[7]))
    {
        tc->welcome_pmt = strdup(argv[7]);
    }
    if (!IS_NULL(argv[8]) && strlen(argv[8]))
    {
        tc->ann_pmt = strdup(argv[8]);
    }
    tc->p_anc = atoi(argv[9]);
    tc->p_anc_time = atoi(argv[10]);
    tc->play_pos_on_call = atoi(argv[11]);
    tc->play_pos_prdcly = atoi(argv[12]);
    if (!IS_NULL(argv[13]) && strlen(argv[13]))
    {
        tc->ann_time = strdup(argv[13]);
    }
    tc->tc_caller_id = atoi(argv[14]);

    return 0;
}

static int sta_init_callback(void *parg, int argc, char **argv, char **column_names)
{
    sta_details_t *sta = (sta_details_t *)parg;
    if (argc < 6)
    {
        switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, " STA  CALLBACK NO DATA %d\n", argc);
        return -1;
    }
    for (int i = 0; i < argc; i++)
        switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, "INTERNAL STA %d %s %s\n", i, column_names[i], argv[i]);

    sta->is_init = true;

    if (!IS_NULL(argv[0]) && strlen(argv[0]))
    {
        sta->id = strdup(argv[0]);
    }

    if (!IS_NULL(argv[1]) && strlen(argv[1]))
    {
        sta->agnt = strdup(argv[1]);
    }
    sta->rcd = atoi(argv[2]);
    sta->sat = atoi(argv[3]);
    if (!IS_NULL(argv[4]) && strlen(argv[4]))
    {
        sta->sticky_start_tm_que = strdup(argv[4]);
    }
    if (!IS_NULL(argv[5]) && strlen(argv[5]))
    {
        sta->sticky_status_que = strdup(argv[5]);
    }

    return false;
}

void handle_play_bck(switch_channel_t *channel, char *dsn, switch_mutex_t *mutex, char *path, char *custom_path, char *id)
{

    switch_core_session_t *session = switch_channel_get_session(channel);
    char *tmp = NULL;
    char *sql = NULL;
    char result[1000] = {'\0'};
const char *result_pin = NULL;
    char digit[50];

    sql = switch_mprintf("select file_path from pbx_prompts where id=%s", id);
    switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, "Dial_string : %s\n", sql);
    execute_sql2str(dsn, mutex, sql, result, NELEMS(result));
//switch_core_session_execute_application(session, "info",NULL);
    // default prompt needed
    
   
    tmp = switch_mprintf("%s%s", custom_path,result);
    switch_play_and_get_digits(session, 1,1, 1, 5000, "#", tmp, NULL, NULL, digit, sizeof(digit), "[0-9]+", 3000, NULL);
    result_pin = strdup(digit);
    switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, "Dial_string : %s %s\n", result,result_pin);
    switch_safe_free(sql);
    switch_safe_free(tmp);
    return;
}

void handle_queue(switch_channel_t *channel, char *dsn, switch_mutex_t *mutex, char *path, char *custom_path, call_details_t *call, int i)
{
    if (clr_status(channel))
    {
        char *wel_pmt = NULL;
        char *sql = NULL;
        char *tmp = NULL;
        char *tc = NULL;
        char tc_mnt[20];
        char *temp_path = NULL;
        originated_header result;
        char caller[30] = {'\0'};
        char tc_unauth[11] = {'\0'};
        char cg_active_id[20] = {'\0'};
        char *contact = NULL;
        const char *dialstatus;
        bool init = true;
        char *tmp_str;
        char *pin;
        char *bs_ivr = NULL;
        char digit_buffer[128] = "1";
        char sticky_buffer[12];
        char clr_no[30];
        const char *opsip_ip_port = switch_channel_get_variable(channel, "opsip_ip_port");
        const char *call_type = switch_channel_get_variable(channel, "call_type");
        switch_core_session_t *session = switch_channel_get_session(channel);
        switch_stream_handle_t stream = {0};
        const char *src = switch_channel_get_variable(channel, "sip_from_user");

        switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, "QUEUE DETAILS : %d", call->did.dst_id);

        switch (i)
        {
        case 1:

            sql = switch_mprintf("call switch_handle_queue_1(%d,'%s')", call->did.dst_id, switch_channel_get_variable(channel, "cust_id"));
            execute_sql_callback(dsn, mutex, sql, cq_init_callback, &call->cq);
            switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, "  switch_handle_queue_1 sql : %s  sticky %d", sql, call->cq.stcky_agnt);
            switch_safe_free(sql);

            if (call->cq.recording)
            {
                switch_channel_set_variable(channel, "set_recording", "1");
            }
            switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, "  stciky agent : %s ", call->sta.agnt);

            if (call->cq.stcky_agnt)
            {
                strcpy(clr_no, switch_channel_get_variable(channel, "ani"));
                result = originated_id_manipulation(channel, clr_no, clr_no, call);
                switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, "originated_id_value: %s\n", result.originated_id_value);
                contact = switch_mprintf("call switch_handle_stcky_agent(%d,'%s','%s')", call->did.dst_id, switch_channel_get_variable(channel, "cust_id"), src);
                switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, "Dial_string : %s\n", contact);
                execute_sql_callback(dsn, mutex, contact, sta_init_callback, &call->sta);
                if (!IS_NULL(call->sta.agnt))
                {
                    if (atoi(call->sta.sticky_status_que) == 1)
                    {
                        switch_play_and_get_digits(session, 1, 1, 1, 15000, "#", bs_ivr, NULL, NULL, sticky_buffer, sizeof(sticky_buffer), "[0-9]+", 2000, NULL);
                        switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, "Dial_string : %s\n", sticky_buffer);
                        if (!strcmp(sticky_buffer, "1"))
                        {
                            if (call->sta.rcd)
                            {
                                call->caller.cust_id = strdup(call->did.cust_id);
                                strcpy(call->caller.num, src);
                                // switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, "Dial_string : %s\n", tmp_str);
                                set_recording(channel, "Call Queue Sticky Agent", call, dsn, mutex, path, custom_path);
                            }

                            for (int i = 0; i < 3; i++)
                            {
                                switch_channel_set_variable(channel, "call_type", "call_sticky_agent");
                                switch_channel_set_variable(channel, "ref_id", call->sta.id);
                                tmp_str = switch_mprintf("{originate_timeout=10,origination_caller_id_number='%s',origination_caller_id_name='%s'}sofia/internal/%s@%s", result.originated_id_value, result.originated_id_value, call->sta.agnt, opsip_ip_port);
                                switch_core_session_execute_application(session, "bridge", tmp_str);

                                switch_safe_free(tmp_str);
                                dialstatus = switch_channel_get_variable(channel, "DIALSTATUS");
                                switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, "sticky type-->: %d\n", call->sta.sat);

                                if ((dialstatus != NULL) && (!strcmp(dialstatus, "BUSY") || !strcmp(dialstatus, "NOANSWER")) && i == 2)
                                {
                                    switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, " cause1: %d\n --->", call->sta.sat);

                                    if (call->sta.sat == 2)

                                    {
                                        switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, " cause1: %d\n", call->sta.sat);
                                        switch_channel_answer(channel);
                                        get_ext_details(channel, &call->callee, dsn, mutex, call->sta.agnt);
                                        switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_DEBUG, ">callee.is_vm_on : %d\n", call->callee.is_vm_on);
                                        if (call->callee.is_vm_on)
                                        {

                                            switch_play_and_get_digits(session, 1, 1, 1, 10000, "#", tmp, NULL, NULL, digit_buffer, sizeof(digit_buffer), "[0-9]+", 10000, NULL);
                                            pin = strdup(digit_buffer);
                                            switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, "Dial_string : %s\n", digit_buffer);
                                            if (init)
                                            {
                                                if (!strcmp(pin, "1"))
                                                {
                                                    voicemail(session, NULL, NULL, call->sta.agnt);
                                                    switch_channel_hangup(channel, SWITCH_CAUSE_CALL_REJECTED);
                                                }
                                                else
                                                {
                                                    init = 0;
                                                }
                                            }
                                        }
                                    }
                                }
                                else
                                {
                                    break;
                                }
                            }

                            if ((dialstatus != NULL) && (!strcmp(dialstatus, "BUSY") || !strcmp(dialstatus, "NOANSWER") || !strcmp(dialstatus, "UNALLOCATED_NUMBER")))
                            {
                                switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, " cause1: %s\n", dialstatus);
                                sleep(10);
                            }
                            else if (dialstatus != NULL && !strcmp(dialstatus, "SUCCESS"))
                            {
                                switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, " causefefefe: %s\n", dialstatus);
                                return;
                            }
                        }
                    }
                }
            }

            switch_channel_set_variable(channel, "ref_id", call->cq.q_name);

            if (call_type == NULL)
            {
                switch_channel_set_variable(channel, "call_type", "call_queue");
            }
            else
            {
                switch_channel_set_variable(channel, "call_type", "call_plugin");
            }

            switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_WARNING, "call_type  : %s", switch_channel_get_variable(channel, "call_type"));
            if (call->cq.is_init == false)
            {
                switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, "queue is not activated now");

                switch_channel_set_variable(channel, "custom_hangup", "1018");
                temp_path = switch_mprintf("%s%s!%s%s", path, "ivr-not_have_permission.wav", path, "invalid_queue.wav");
                switch_channel_set_variable(channel, "playback_delimiter", "!");
                // temp_path = switch_mprintf("%s%s", path, "");
                switch_ivr_play_file(session, NULL, temp_path, NULL);
                switch_channel_hangup(channel, SWITCH_CAUSE_CALL_REJECTED);
                return;
            }
            break;

        case 2:
            switch_channel_set_variable(channel, "call_type", "call_tc");
            tc = switch_mprintf("call switch_handle_tele_consultation('%s',%d,'%s')", src, call->did.dst_id, call->did.cust_id);
            switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, " tc SQL : %s\n", tc);
            execute_sql_callback(dsn, mutex, tc, tc_init_callback, &call->tc);
            switch_safe_free(tc);

            switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, " Auth_call tc mnt  : %d  tc.is_init %d ", call->tc.mnt, call->tc.is_init);
            if (call->tc.is_init)
            {
                if (call->tc.mnt <= 0)
                {
                    temp_path = switch_mprintf("%s%s", path, "balance_exhaust.wav");
                    switch_core_session_execute_application(session, "playback", temp_path);
                    switch_channel_hangup(channel, SWITCH_CAUSE_CALL_REJECTED);
                    return;
                }
                else
                {
                    switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, " Auth_call Ann_time  : %s\n", call->tc.ann_time);
                    sprintf(tc_mnt, "%d", call->tc.mnt);
                    switch_channel_set_variable(channel, "tc_mnt", tc_mnt);
                    call->flags.callerid_as_DID = call->tc.tc_caller_id;
                    switch_channel_set_variable(channel, "is_auth_tc", "1");
                    switch_channel_set_variable(channel, "tc_mnt_id", call->tc.mnt_id);
                    switch_channel_set_variable(channel, "ref_id", call->tc.id);
                    switch_channel_set_variable(channel, "ann_time", call->tc.ann_time);
                    strcpy(call->cq.q_name, call->tc.tc_name);
                    call->cq.welcome_pmt = call->tc.welcome_pmt;
                    call->cq.p_anc = call->tc.p_anc;
                    call->cq.p_anc_time = call->tc.p_anc_time;
                    call->cq.max_wait_call = call->tc.max_wait_call;
                }
            }
            else
            {
                sql = switch_mprintf("INSERT into pbx_tc_unauth_call (tc_id,src,dest,customer_id,start_time) VALUES(%d, '%s','%s','%s',CURRENT_TIMESTAMP())", call->did.dst_id, src, call->did.num, call->did.cust_id);
                execute_sql2str(dsn, mutex, sql, tc_unauth, NELEMS(tc_unauth));
                // switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, "pbx_tc_unauth_call isql : %s\n", sql);
                switch_safe_free(sql);
                
                sql = switch_mprintf(" call switch_tc_unauthorized('%s',%d,'%s','%s')", src, call->did.dst_id, call->did.cust_id, call->did.country_prefix);
                execute_sql_callback(dsn, mutex, sql, un_tc_init_callback, &call->un_tc);

                call->flags.callerid_as_DID = call->un_tc.tc_caller_id;
                switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, "unauth  id %s remaining_mnt %d \n", call->un_tc.id, call->un_tc.mnt);
                switch_safe_free(sql);
                if (call->un_tc.mnt)
                {
                    sprintf(tc_mnt, "%d", call->un_tc.mnt);
                    switch_channel_set_variable(channel, "tc_mnt", tc_mnt);
                    switch_channel_set_variable(channel, "is_auth_tc", "0");
                    switch_channel_set_variable(channel, "tc_mnt_id", call->un_tc.id);
                    strcpy(call->cq.q_name, call->un_tc.tc_name);
                    call->cq.welcome_pmt = call->un_tc.welcome_pmt;
                    call->cq.p_anc = call->un_tc.p_anc;
                    call->cq.p_anc_time = call->un_tc.p_anc_time;
                    call->cq.max_wait_call = call->un_tc.max_wait_call;
                    switch_channel_set_variable(channel, "ref_id", call->un_tc.id);
                }
                else if (call->un_tc.unauth_failover)
                {
                    temp_path = switch_mprintf("%s%s", path, "tc_unauthorize_failover.wav");
                    switch_core_session_execute_application(session, "playback", temp_path);
                    sql = switch_mprintf("SELECT active_feature,active_feature_value FROM `pbx_tc` where id=%d", call->did.dst_id);
                    execute_sql_callback(dsn, mutex, sql, tf_init_callback, &call->tf);
                    sprintf(cg_active_id, "%d", call->tf.actve_val);
                    switch_channel_set_variable(channel, "cg_active_id", cg_active_id);
                    switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, " SQL Failover Call Group: %s\n  cg active id::%d cg_active_id:%s", sql, call->tf.actve_val, cg_active_id);
                    handle_tc_failover(channel, dsn, mutex, path, custom_path, call, call->tc.id, call->tf.actve_ftre, call->tf.actve_val);
                }
                else
                {
                    temp_path = switch_mprintf("%s%s", path, "tc_unauthorize.wav");
                    switch_core_session_execute_application(session, "playback", temp_path);
                    switch_channel_hangup(channel, SWITCH_CAUSE_CALL_REJECTED);
                    return;
                }
                break;
            }
        default:
            break;
        }
       
        SWITCH_STANDARD_STREAM(stream);
        tmp = switch_mprintf("queue count members %s", call->cq.q_name);
        switch_api_execute("callcenter_config", tmp, NULL, &stream);
        switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, " CQ SQL : %d", atoi((char *)stream.data));
        switch_safe_free(tmp);

        if (atoi((char *)stream.data) >= call->cq.max_wait_call)
        {
            switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, " Call QUEUE %s full", call->cq.q_name);
            // switch_channel_hangup(channel, SWITCH_CAUSE_CALL_REJECTED);
            return;
        }
        switch_safe_free(stream.data);

        handle_default_prompts(call, call->cq.welcome_pmt, NULL, NULL, path, custom_path);
        wel_pmt = call->dp.wel_pmt;
        call->cq.ann_pmt = call->dp.wel_pmt;
        switch_core_session_execute_application(session, "playback", wel_pmt);

        if (call->cq.p_anc)
        {
            char *uuid = switch_channel_get_uuid(channel);
            // tmp = switch_mprintf("result=${luarun(/usr/local/freeswitch/scripts/callcenter-anouncement-position.lua %s %s@default %d %s %d)}", uuid, call->cq.q_name, (call->cq.p_anc_time * 1000), (call->cq.ann_pmt), call->cq.play_pos_on_call);
            tmp = switch_mprintf("result=${luarun(/usr/local/freeswitch/scripts/callcenter-anouncement-position.lua %s %s@default %d %d )}", uuid, call->cq.q_name, (call->cq.p_anc_time * 1000), call->cq.play_pos_on_call);
            switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, "CALL Periodic Ann: %d %s.\n", call->cq.p_anc_time, tmp);
            switch_core_session_execute_application(session, "set", tmp);
            switch_safe_free(tmp);
        }

        strcpy(caller, switch_channel_get_variable(channel, "ani"));
        result = originated_id_manipulation(channel, caller, caller, call);
        switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_WARNING, "originated_id_value: %s did %s\n", result.originated_id_value, call->did.num);
        tmp = switch_mprintf("%s@default", call->cq.q_name);
        switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, " CQ NAME : %s\n", tmp);
        switch_core_session_execute_application(session, "callcenter", tmp);
        switch_safe_free(tmp);

        switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, "queue hangup reason: %s\n", switch_channel_get_variable(channel, "cc_cancel_reason"));
        if (switch_channel_get_variable(channel, "cc_cancel_reason") != NULL && !strcasecmp(switch_channel_get_variable(channel, "cc_cancel_reason"), "TIMEOUT") && call->cq.is_init)
        {
            switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_WARNING, " unauth_usr voicemail: %s\n", call->cq.unauth_usr);
            voicemail(session, NULL, NULL, call->cq.unauth_usr);
        }
        switch_safe_free(temp_path)
    }
    return;
}

static int cv_init_callback(void *parg, int argc, char **argv, char **column_names)
{
    ivr_details_t *cv = (ivr_details_t *)parg;
    if (argc < 11)
    {
        switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, " CG CALLBACK ERROR : NO DATA %d\n", argc);
        return -1;
    }
    for (int i = 0; i < argc; i++)
        // switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, "Call ivr: %d %s  %s \n", i,column_names[i], argv[i]);

        cv->is_init = true;
    if (!IS_NULL(argv[0]) && strlen(argv[0]))
    {
        strcpy(cv->ivr_name, argv[0]);
    }
    cv->timeout = atoi(argv[1]);
    cv->dgt_timeout = atoi(argv[2]);
    cv->mx_timeout = atoi(argv[3]);
    cv->invld_count = atoi(argv[4]);
    cv->drct_ext_call = atoi(argv[5]);
    if (!IS_NULL(argv[6]) && strlen(argv[6]))
    {
        strcpy(cv->ivr_dgt, argv[6]);
    }
    if (!IS_NULL(argv[7]) && strlen(argv[7]))
    {
        strcpy(cv->ivr_prm, argv[7]);
    }
    if (!IS_NULL(argv[8]) && strlen(argv[8]))
    {
        cv->welcome_pmt = strdup(argv[8]);
    }
    if (!IS_NULL(argv[9]) && strlen(argv[9]))
    {
        cv->repeat_pmt = strdup(argv[9]);
    }

    if (strlen(argv[10]) && !IS_NULL(argv[10]))
    {

        cv->invalid_pmt = strdup(argv[10]);
        // cv->invalid_pmt  =strdup("/var/www/html/fs_backend/upload/def_prompts/invalid_digit.wav");
    }
    /* else{
    cv->invalid_pmt =  switch_mprintf("/var/www/html/pbx/app/%s",argv[10]);

    } */

    if (!IS_NULL(argv[11]) && strlen(argv[11]))
    {
        cv->timeout_pmt = strdup(argv[11]);
    }
    /*
    else{
       cv->timeout_pmt =strdup("/var/www/html/fs_backend/upload/def_prompts/max_tries.wav");
    } */
    cv->is_direct_ext_dial = atoi(argv[12]);

    return 0;
}

void handle_ivr(switch_channel_t *channel, char *dsn, switch_mutex_t *mutex, char *path, char *custom_path, call_details_t *call, struct stack *pt, int ivr)
{

    if (clr_status(channel))
    {

        char *temp_path;
        char *tmp;
        // char *default_pmpt ;
        char *ask_extn = NULL;
        char *wrng_extn = NULL;
        char *contact = NULL;
        char *sql;
        char *vm = NULL;
        struct node *head = NULL;
        struct node *a = NULL;
        // struct stack *pt = newStack(5);
        char *ivr_pin, *digit;
        int i, k, n, num, list;
        char **tokens = NULL;
        char **tok = NULL;
        char digit_buffer[128] = "";

        char **tokl = NULL;
        const char *number;
        const char *callee;
        char result[20] = {'\0'};
        switch_core_session_t *session = switch_channel_get_session(channel);
        const char *caller = switch_channel_get_variable(channel, "sip_from_user");
        const char *ref_id = switch_channel_get_variable(channel, "ref_id");

        wrng_extn = switch_mprintf("%s%s", path, "wrng_extn.wav");
        ask_extn = switch_mprintf("%s%s", path, "extn_provide_1.wav");
        switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, "IVR shedule_bkp feedback ref_id: %s", ref_id);

        sql = switch_mprintf("call switch_handle_ivr(%d,'%s')", call->did.dst_id, ref_id);
        switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, "IVR Option configuration start :: SQL : %s", sql);
        execute_sql_callback(dsn, mutex, sql, cv_init_callback, &call->cv);
        switch_safe_free(sql);

        handle_default_prompts(call, call->cv.welcome_pmt, call->cv.invalid_pmt, call->cv.timeout_pmt, path, custom_path);
        call->cv.invalid_pmt = call->dp.invalid_pmt;
        call->cv.timeout_pmt = call->dp.time_out_pmt;
        call->cv.welcome_pmt = call->dp.wel_pmt;

        if (!strcmp(call->cv.repeat_pmt, "0") || !file_exists(switch_mprintf("%s%s", custom_path, call->cv.repeat_pmt)))
        {
            call->cv.repeat_pmt = switch_mprintf("%s%s", path, "default_prompt.wav");
        }
        else
        {
            call->cv.repeat_pmt = switch_mprintf("%s%s", custom_path, call->cv.repeat_pmt);
        }
        switch_core_session_execute_application(session, "playback", call->cv.welcome_pmt);

        list = ((strlen(call->cv.ivr_dgt) + 1) / 2);
        tokens = split(call->cv.ivr_dgt, &num, ",", channel);
        tok = split(call->cv.ivr_prm, &n, ",", channel);

        for (k = 0; k < list; k++)
        {
            addLast(&head, tokens[k], tok[k]);
        }
        printList(head);

        // default_pmpt = switch_mprintf("%s%s", path, "default_prompt.wav");

        if (call->cv.is_direct_ext_dial)
        {
            tmp = switch_mprintf("%s%s", path, "extn_no_feature_call.wav");
            switch_play_and_get_digits(session, 1, 1, 3, (call->cv.timeout) * 1000, "", tmp, NULL, NULL, digit_buffer,
                                       sizeof(digit_buffer), "[0-9,*,#]+", (call->cv.dgt_timeout) * 1000, NULL);
            switch_safe_free(tmp);
            digit = strdup(digit_buffer);
            if (atoi(digit) == 0)
            {
                digit = "2";
            }
        }
        else
        {
            digit = "2";
        }
        switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, "direct_ext_dial is 0 so 2 is set for digit : %d\n ", atoi(digit));

        if (atoi(digit) == 1)
        {
            for (i = 1; i <= call->cv.invld_count; i++)
            {

                tmp = switch_mprintf("%s", ask_extn);
                /*tmp = switch_mprintf("%s!%s%s" ,ask_extn,path,"followed_byhash.wav");
                switch_channel_set_variable(channel, "playback_delimiter", "!"); */

                switch_play_and_get_digits(session, 1, 8, 1, (call->cv.timeout) * 1000, "#", tmp, NULL, NULL, digit_buffer,
                                           sizeof(digit_buffer), "[1-9,*,#]+", (call->cv.dgt_timeout) * 1000, NULL);
                 switch_channel_set_variable(channel, "dtmf", digit_buffer);
                if (strlen(digit_buffer) >= 2)
                {
                    if (strlen(digit_buffer) >= 2 && strlen(digit_buffer) <= 4)
                    {
                        callee = switch_mprintf("%s%s", call->did.cust_id, digit_buffer);
                    }
                    else
                    {
                        callee = switch_mprintf("%s", digit_buffer);
                    }
                    switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, "CALL TYPE :%s\n", callee);
                    switch_channel_set_variable(channel, "call_type", "sip_extn");
                    switch_channel_set_variable(channel, "application", "inbound");
                    switch_channel_set_variable(channel, "cust_id", call->did.cust_id);
                    switch_channel_set_variable(channel, "sip_req_user", callee);

                    if (!get_ext_details(channel, &call->callee, dsn, mutex, callee))
                    {
                        switch_ivr_play_file(session, NULL, wrng_extn, NULL);

                        if (i == call->cv.invld_count)
                        {
                            temp_path = switch_mprintf("%s%s", path, "max_tries.wav");
                            switch_ivr_play_file(session, NULL, temp_path, NULL);
                            switch_safe_free(temp_path);
                            switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, "max tries \n");
                            break;
                        }

                        continue;
                    }
                    else
                    {

                        call->caller.cust_id = call->did.cust_id;
                        handle_sip_call(channel, dsn, mutex, path, custom_path, call);
                        switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_DEBUG, " count %d\n", i);

                        switch_channel_hangup(channel, SWITCH_CAUSE_CALL_REJECTED);
                        break;
                    }
                }
                // sleep(call->cv.timeout);
                switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, " SIP Extens :  timeout %d\n", call->cv.timeout);
            }
        }

        if (atoi(digit) >= 2)
        {
            for (i = 1; i <= call->cv.invld_count; i++)
            {
                switch_play_and_get_digits(session, 1, 1, 1, (call->cv.timeout) * 1000, "", call->cv.repeat_pmt, NULL, NULL, digit_buffer,
                                           sizeof(digit_buffer), "[0-9,*,#]+", (call->cv.dgt_timeout) * 1000, NULL);

                ivr_pin = strdup(digit_buffer);
                switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_DEBUG, "  ivr_pin : %zu\n", strlen(ivr_pin));
                 switch_channel_set_variable(channel, "dtmf", ivr_pin);

                if (!strcmp(ivr_pin, ""))
                {
                    switch_channel_set_variable(channel, "call_dtmf", "no_input");

                    if (strlen(ivr_pin) != 0)
                    {
                        temp_path = call->cv.invalid_pmt;
                    }
                    else if (i < call->cv.invld_count)
                    {
                        temp_path = switch_mprintf("%s%s", path, "no_input.wav");
                    }
                    else
                    {
                        temp_path = call->cv.timeout_pmt;
                    }

                    switch_ivr_play_file(session, NULL, temp_path, NULL);
                    switch_safe_free(temp_path);
                    continue;
                }

                a = search(head, ivr_pin);
                if (IS_NULL(a))
                {
                    switch_channel_set_variable(channel, "call_dtmf", "invalid_input");
                    if (strcmp(call->cv.invalid_pmt, ""))
                    {
                        switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, " CQ NAME : %s\n tries:%i ", call->cv.invalid_pmt, i);
                        tmp = call->cv.invalid_pmt;
                        if (i == call->cv.invld_count)
                        {
                            temp_path = switch_mprintf("%s%s", path, "max_tries.wav");
                            switch_ivr_play_file(session, NULL, temp_path, NULL);
                            switch_safe_free(temp_path);
                            switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_DEBUG, "max tries ");
                            break;
                        }
                    }

                    else
                    {
                        temp_path = switch_mprintf("%s%s", path, "invalid_digit.wav");
                        switch_ivr_play_file(session, NULL, temp_path, NULL);
                        switch_safe_free(temp_path);
                    }
                    switch_ivr_play_file(session, NULL, tmp, NULL);

                    continue;
                }
                else
                {
                
                switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, "IVR Details after DTMF selection :%s", a->key);
                tokl = split(a->key, &n, "_", channel);
                switch_channel_set_variable(channel, "cg_active_id", a->key + 3);
                switch_channel_set_variable(channel, "call_dtmf", ivr_pin);

                if (!strcasecmp(tokl[0], "conf"))
                {
                    switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, " cnof1:\n");
                    call->did.dst_id = atoi(tokl[1]);
                    if (verify_internal_exten(channel, dsn, mutex, call, tokl[1], path))
                    {
                        switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, " cnof2:\n");

                        if (call->conf.is_init)
                        {

                            handle_conf(channel, call, dsn, mutex, path, custom_path);
                        }
                    }
                    break;
                }
                else if (!strcasecmp(tokl[0], "queue"))
                {
                    call->did.dst_id = atoi(tokl[1]);
                    handle_queue(channel, dsn, mutex, path, custom_path, call, 1);
                    break;
                }
                else if (!strcasecmp(tokl[0], "cg"))
                {
                    call->did.dst_id = atoi(tokl[1]);
                    if (verify_internal_exten(channel, dsn, mutex, call, tokl[1], path))
                    {
                        if (call->cg.is_init)
                        {
                            call->caller.cust_id = call->did.cust_id;
                            sscanf(caller, "%s", call->caller.num);
                            handle_cg(channel, call, dsn, mutex, path, custom_path);
                        }
                    }
                    break;
                }
                else if (!strcasecmp(tokl[0], "sip"))
                {

                    call->flags.is_inbound_sip = true;
                    switch_channel_set_variable_printf(channel, "ext_id", "%s", tokl[1]);
                    if (get_ext_details(channel, &call->callee, dsn, mutex, tokl[1]))
                    {
                        handle_sip_call(channel, dsn, mutex, path, custom_path, call);
                    }

                    break;
                }
                else if (!strcasecmp(tokl[0], "custom"))
                {
                    if (call->did.is_outbound_on == true)
                    {
                        call->flags.is_call_outbound = true;
                        call->caller.cust_id = call->did.cust_id;
                        number = tokl[2];
                        switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, " custom number : %s\n", number);
                        if (dialoutrule(channel, dsn, mutex, path, custom_path, call, number) && outbound(session, dsn, mutex, path, custom_path, call, number) && (call->obd.is_init || call->flags.is_outbound_mnt || call->flags.is_outbound_grp_mnt))
                        {
                            bridge_call(session, call, number, dsn, mutex);
                        }
                    }
                    else
                    {
                        switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, " In-side ELSE:: check custom number : %s\n", tokl[2]);
                    }

                    break;
                }
                else if (!strcasecmp(tokl[0], "vm"))
                {

                    vm = switch_mprintf("select ext_number from  extension_master where  id =%s", tokl[1]);
                    execute_sql2str(dsn, mutex, vm, result, NELEMS(result));

                    if (get_ext_details(channel, &call->caller, dsn, mutex, result))
                    {
                        if (call->caller.is_vm_on)
                        {
                            voicemail(session, NULL, NULL, result);
                        }
                        else
                        {
                            sleep(1);

                            temp_path = switch_mprintf("%s%s", path, "forward_vm_blocked.wav");
                            switch_ivr_play_file(session, NULL, temp_path, NULL);
                            switch_safe_free(temp_path);
                        }
                        switch_safe_free(vm);
                    }
                    break;
                }
                else if (!strcasecmp(tokl[0], "contact"))
                {
                    switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, "contact ");

                    call->caller.cust_id = switch_mprintf("%s", switch_channel_get_variable(channel, "cust_id"));
                    call->flags.is_call_outbound = true;

                    contact = switch_mprintf("select phone_number1 from pbx_contact_list where id=%s", tokl[2]);
                    execute_sql2str(dsn, mutex, contact, result, NELEMS(result));
                    if (dialoutrule(channel, dsn, mutex, path, custom_path, call, result) && outbound(session, dsn, mutex, path, custom_path, call, result) && (call->obd.is_init || call->flags.is_outbound_mnt || call->flags.is_outbound_grp_mnt))
                    {
                        bridge_call(session, call, result, dsn, mutex);
                    }
                    switch_safe_free(contact);

                    break;
                }
                else if (!strcasecmp(tokl[0], "playback"))
                {
                    handle_play_bck(channel, dsn, mutex, path, custom_path, tokl[1]);
                    break;
                }
                else if (!strcasecmp(tokl[0], "ivr"))
                {
                    push(pt, call->did.dst_id);
                    call->did.dst_id = atoi(tokl[1]);
                    switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, "The top element is %d\n", peek(pt));

                    handle_ivr(channel, dsn, mutex, path, custom_path, call, pt, 1);
                    break;
                }
                else if (!strcasecmp(tokl[0], "back2ivr"))
                {
                    if (!isEmpty(pt))
                    {
                        call->did.dst_id = pop(pt);
                        handle_ivr(channel, dsn, mutex, path, custom_path, call, pt, 0);

                        switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, "Empty IVR: %d\n", call->did.dst_id);
                    }
                    break;
                }

                else if (!strcmp(tokl[0], "0"))
                {
                    switch_channel_set_variable(channel, "custom_hangup", "200");
                    return;
                }
                else if (!strcmp(tokl[0], "-1"))
                {
                    continue;
                }
            }
              }

        }
        // switch_core_session_execute_application(session, "info",NULL);
        switch_safe_free(tokens);
        switch_safe_free(tok);
        switch_safe_free(tokl);
        return;
    }
}

void handle_prompt(switch_channel_t *channel, const char *dialstatus, char *path, char *custom_path)
{
    if (clr_status(channel) && !IS_NULL(dialstatus))
    {

        char *temp_path = NULL;

        switch_core_session_t *session = switch_channel_get_session(channel);

        if (!strcmp(dialstatus, "BUSY") || !strcmp(dialstatus, "CANCEL"))
        {

            temp_path = switch_mprintf("%s%s", path, "call_rejected.wav");
            switch_core_session_execute_application(session, "playback", temp_path);
        }
        else if (!strcmp(dialstatus, "NOANSWER") || !strcmp(dialstatus, "ALLOTTED_TIMEOUT"))
        {

            temp_path = switch_mprintf("%s%s", path, "ivr-no_user_response.wav");
            switch_core_session_execute_application(session, "playback", temp_path);
        }
        else if (!strcmp(dialstatus, "UNALLOCATED_NUMBER"))
        {
            temp_path = switch_mprintf("%s%s", path, "ivr-unallocated_number.wav");
            switch_core_session_execute_application(session, "playback", temp_path);
        }
        else if (!strcmp(dialstatus, "NORMAL_UNSPECIFIED"))
        {
            temp_path = switch_mprintf("%s%s", path, "user_is_unavailable.wav");
            switch_core_session_execute_application(session, "playback", temp_path);
        }
        else if (!strcmp(dialstatus, "DONTCALL") || !strcmp(dialstatus, "INCOMPATIBLE_DESTINATION"))
        {
            temp_path = switch_mprintf("%s%s", path, "Dont_call.wav");
            switch_core_session_execute_application(session, "playback", temp_path);
        }
        else if (!strcmp(dialstatus, "FAILED_TO_GET_EXTN"))
        {
            temp_path = switch_mprintf("%s%s", path, "ivr-please_check_number_try_again.wav");
            switch_core_session_execute_application(session, "playback", temp_path);
        }
        else if (!strcmp(dialstatus, "NO_ROUTE_DESTINATION"))
        {
            temp_path = switch_mprintf("%s%s", path, "NO_ROUTE_DESTINATION.wav");
            switch_core_session_execute_application(session, "playback", temp_path);
        }

        switch_safe_free(temp_path)
    }
}




// split function
char **split(char string[], int *num, char *sep, switch_channel_t *channel)
{

    char *pch;
    char **out = 0;
    int i = 0;
    pch = strtok(string, sep);

    while (pch != 0)
    {
        out = realloc(out, (i + 1) * sizeof(char *));
        out[i] = malloc(strlen(pch) + 1);
        strcpy(out[i], pch);
        ++i;
        pch = strtok(NULL, sep);
    }
    *num = i;
    switch_channel_set_variable(channel, "split_index", switch_mprintf("%d", i));
    // switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_ERROR   ," index-->%s ",switch_channel_get_variable(channel, "split_index"));
    return out;
}

// add  element
void addLast(struct node **head, char *val, char *key)
{

    struct node *newNode = malloc(sizeof(struct node));
    newNode->data = val;
    newNode->key = key;
    newNode->next = NULL;

    if (*head == NULL)
        *head = newNode;

    else
    {
        struct node *lastNode = *head;

        while (lastNode->next != NULL)
        {
            lastNode = lastNode->next;
        }

        lastNode->next = newNode;
    }
}

struct node *search(struct node *head, char *key)
{

    struct node *curNode;
    curNode = head;

    while (curNode != NULL)
    {

        if (!strcmp(curNode->data, key))
        {
            return curNode;
        }
        else
            curNode = curNode->next;
    }
    return NULL;
}

void printList(struct node *head)
{
    struct node *temp = head;

    // iterate the entire linked list and print the data
    while (temp != NULL)
    {
        // switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_ERROR   ,"%s->", temp->data);
        // switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_ERROR       , "%s->",temp->key);
        temp = temp->next;
    }
}

// stack database
struct stack *newStack(int capacity)
{
    struct stack *pt = (struct stack *)malloc(sizeof(struct stack));
    pt->maxsize = capacity;
    pt->top = -1;
    pt->items = (int *)malloc(sizeof(int) * capacity);

    return pt;
}

int size(struct stack *pt)
{
    return pt->top + 1;
}

int isEmpty(struct stack *pt)
{
    return pt->top == -1;
}

int isFull(struct stack *pt)
{
    return pt->top == pt->maxsize - 1;
}

void push(struct stack *pt, int x)
{
    if (isFull(pt))
    {
        printf("Overflow\nProgram Terminated\n");
        exit(EXIT_FAILURE);
    }

    printf("Inserting %d\n", x);
    pt->items[++pt->top] = x;
}

int peek(struct stack *pt)
{

    if (!isEmpty(pt))
    {
        return pt->items[pt->top];
    }
    else
    {
        exit(EXIT_FAILURE);
    }
}

int pop(struct stack *pt)
{

    if (isEmpty(pt))
    {
        printf("Underflow\nProgram Terminated\n");
        exit(EXIT_FAILURE);
    }

    printf("Removing %d\n", peek(pt));

    return pt->items[pt->top--];
}
