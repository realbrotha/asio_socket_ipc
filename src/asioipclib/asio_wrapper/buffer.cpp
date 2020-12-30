//
// Created by realbro on 19. 6. 18.
//

#include "buffer.h"
#include <cstring>

namespace
{
const size_t g_buff_reserve_unit_size_default = 2048;
}

namespace network {

buffer::buffer()
{
    buff_.resize(g_buff_reserve_unit_size_default);
    reserve_size_unit_ = g_buff_reserve_unit_size_default;
    size_ = 0;
}

buffer::buffer(const uint8_t *buff, const size_t len)
{
    if (len == 0)
    {
        buff_.resize(g_buff_reserve_unit_size_default);
        size_ = 0;
    }
    else
    {
        buff_.resize(len);
        memcpy(&buff_[0], buff, len);
        size_ = len;
    }
    reserve_size_unit_ = g_buff_reserve_unit_size_default;
}
void buffer::reset() {
  buff_.resize(g_buff_reserve_unit_size_default);
  reserve_size_unit_ = g_buff_reserve_unit_size_default;
  size_ = 0;
}
void buffer::set_reserve_size_unit(const size_t size)
{
    if (size < g_buff_reserve_unit_size_default)
    {
        return;
    }
    else if (size > g_buff_reserve_unit_size_default*2)
    {
        reserve_size_unit_ = g_buff_reserve_unit_size_default*2;
    }
    reserve_size_unit_ = size;
}

size_t buffer::get_reserve_size_unit() const
{
    return reserve_size_unit_;
}

size_t buffer::size() const
{
    return size_;
}

uint8_t* buffer::get()
{
    return &buff_[0];
}

uint8_t* buffer::get(const size_t len)
{
    if (len == 0)
    {
        return NULL;
    }

    if (buff_.size() < len)
    {
        buff_.resize(len);
        size_ = len;
        return &buff_[0];
    }

    // default max size 보다 크게 할당 되어있을 경우 읽기-버퍼 사이즈를 줄인다.
    if (buff_.size() > reserve_size_unit_ &&
        len <= reserve_size_unit_)
    {
        buff_.resize(reserve_size_unit_);
    }
    size_ = len;

    return &buff_[0];
}

void buffer::copy(const uint8_t *buff, const size_t len)
{
    copy(0, buff, len);
}

void buffer::copy(const size_t offset, const uint8_t* buff, const size_t len)
{
    if (len == 0 || buff == NULL)
    {
        return;
    }

    if (buff_.size() < len + offset)
    {
        buff_.resize(len + offset);
    }

    if (buff_.size() > reserve_size_unit_ &&
        len <= reserve_size_unit_)
    {
        buff_.resize(reserve_size_unit_);
    }
    memcpy(&buff_[offset], buff, len);
    size_ = len + offset;
}

void buffer::append(const void* data, const size_t data_size)
{
    if (data == NULL || data_size == 0)
    {
        return;
    }
    if (buff_.size() < size_ + data_size)
    {
        buff_.resize(buff_.size() + reserve_size_unit_);
    }
    memcpy((&buff_[0]) + size_, data, data_size);
    size_ += data_size;
}
}
