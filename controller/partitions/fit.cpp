#include <iostream>
#include <string.h>
#include "../../model/structures.h"
#include "../handler.h"
#include "../disks/func.h"
#include <vector>

int getPartitionByFit(int _options[], char _fit)
{
    int n = _options[0];
    int ret = -1;
    for (int i = 0; i < 4; i++)
    {
        if (_options[i] != -1)
        {
            switch (_fit)
            {
            case 'F':
                return i;
            case 'W':
                if (_options[i] > n || ret == -1)
                {
                    n = _options[i];
                    ret = i;
                }
                break;
            case 'B':
                if (_options[i] < n || ret == -1)
                {
                    n = _options[i];
                    ret = i;
                }
                break;
            default:
                return -1;
            }
        }
    }
    return ret;
}

std::vector<EBR> toRunLogicPartitions(EBR _ebr, FILE *_file, int _size, std::vector<EBR> _list)
{
    if (_ebr.part_next - _ebr.part_start - _ebr.part_size > _size || _ebr.part_next == -1)
        _list.push_back(_ebr);
    if (_ebr.part_next != -1)
    {
        fseek(_file, _ebr.part_next, SEEK_SET);
        fread(&_ebr, sizeof(EBR), 1, _file);
        return toRunLogicPartitions(_ebr, _file, _size, _list);
    }
    return _list;
}

EBR getLogicByFit(EBR _ebr_initial, char _fit, FILE *_file, int _size)
{
    std::vector<EBR> _options = toRunLogicPartitions(_ebr_initial, _file, _size, std::vector<EBR>());
    if (_options.size() == 0)
        return {part_status : '!'};
    int n, aux = -1;
    EBR _ebr, ret = _options[0];
    for (int i = 0; i < _options.size(); i++)
    {
        _ebr = _options[i];
        aux = (_ebr.part_next == -1) ? 0 : _ebr.part_next - _ebr.part_start - _ebr.part_size;
        switch (_fit)
        {
        case 'F':
            return _ebr;
        case 'B':
            if (aux < n)
            {
                n = aux;
                ret = _ebr;
            }
            break;
        case 'W':
            if (aux > n)
            {
                n = aux;
                ret = _ebr;
            }
            break;
        default:
            return {part_status : '!'};
        }
    }
    return ret;
}
