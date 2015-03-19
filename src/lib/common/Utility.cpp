#include "Utility.hpp"

#include <cstdlib>

namespace cigar {

int32_t calculate_right_offset(bam1_t const* record) {
    //htslib doesn't quite do what we want here as softclipping doesn't consume the reference
    //also, samblaster considers leading soft or hardclips in unpacking the coords. But only the first one.
    //TODO What is valid here?
    int32_t offset = 0;
    //TODO handle if we don't have a cigar
    uint32_t *cigar = bam_get_cigar(record);

    int opindex = 0;
    while( (bam_cigar_type(bam_cigar_op(cigar[opindex])) == 0x00 
                || bam_cigar_op(cigar[opindex]) == BAM_CSOFT_CLIP) 
            && opindex < record->core.n_cigar) {
        //NOTE that you could just increment in the while statement.
        //I think this is less prone to misinterpretation
        //This should strip any leading Hardclips or softclips. Note that in the pathological case of a read that is fully hard/softclipped this may not make sense
        opindex++;
    }
    while( opindex < record->core.n_cigar) {
        if(bam_cigar_type(bam_cigar_op(cigar[opindex]))&2 
                || bam_cigar_op(cigar[opindex]) == BAM_CSOFT_CLIP
                || bam_cigar_op(cigar[opindex]) == BAM_CHARD_CLIP) {
            offset += bam_cigar_oplen(cigar[opindex]);
        }
        opindex++;
    }

    return offset - 1;
}

int32_t calculate_right_offset(char const* cigar) {
    int32_t offset = 0;
    char *op_ptr = (char *)cigar;
    long int oplen = -1;
    char opchr;
    bool first = true;
    while(*op_ptr != '\0') {
        oplen = strtol(op_ptr, &op_ptr, 10);
        opchr = *op_ptr;
        op_ptr++;
        if(opchr == 'H' || opchr == 'S') {
            if(!first) {
                offset += oplen;
            }
        }
        else { 
            first = false;
            if(opchr == 'M' || opchr == '=' || opchr == 'X' || opchr == 'D') {
                offset += oplen;
            }
            else if(opchr == 'I') {
                //do nothing. We don't care about insertions here.
            }
            else {
                //blow up
            }
        }
    }
    return offset - 1;
}

int32_t calculate_left_offset(bam1_t const* record) {
    int32_t offset = 0;
    //TODO handle if we don't have a CIGAR
    uint32_t *cigar = bam_get_cigar(record);

    //all we care about is if the start of this guy is softclipped
    //NOTE we are not going to check for properly constructed CIGAR strings
    int opindex = 0;
    while(bam_cigar_type(bam_cigar_op(cigar[opindex])) == 0x00 
            && opindex < record->core.n_cigar) {
        //NOTE that you could just increment in the while statement.
        //I think this is less prone to misinterpretation
        //This should strip any leading 
        opindex++;
    }

    if(bam_cigar_op(cigar[opindex]) == BAM_CSOFT_CLIP) {
        offset -= bam_cigar_oplen(cigar[opindex]);
    }
    return offset;
}
int32_t calculate_left_offset(char const* cigar) {
    int32_t offset = 0;
    char *op_ptr = (char *)cigar;
    long int oplen = -1;
    char opchr;
    while(*op_ptr != '\0') {
        oplen = strtol(op_ptr, &op_ptr, 10);
        opchr = *op_ptr;
        op_ptr++;
        if(opchr == 'H' || opchr == 'S') {
            offset += oplen;
        }
        else { 
            if(opchr == 'M' 
                    || opchr == '=' 
                    || opchr == 'X' 
                    || opchr == 'D'
                    || opchr == 'I') {
                return offset; 
            }
            else {
                //blow up
            }
        }
    }
    return offset;
}

}