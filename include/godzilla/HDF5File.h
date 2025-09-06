// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Enums.h"
#include "godzilla/Types.h"
#include "godzilla/Exception.h"
#include <filesystem>
#include <string>
#include <numeric>
#include <concepts>
#include <mutex>
#include <hdf5.h>

namespace fs = std::filesystem;

namespace godzilla {

template <typename T>
concept StdVector = requires {
    typename T::value_type;
    requires std::same_as<T, std::vector<typename T::value_type, typename T::allocator_type>>;
};

namespace hdf5 {

template <typename T>
hid_t get_datatype();

template <>
inline hid_t
get_datatype<char>()
{
    return H5T_NATIVE_CHAR;
}

template <>
inline hid_t
get_datatype<signed char>()
{
    return H5T_NATIVE_SCHAR;
}

template <>
inline hid_t
get_datatype<unsigned char>()
{
    return H5T_NATIVE_UCHAR;
}

template <>
inline hid_t
get_datatype<short>()
{
    return H5T_NATIVE_SHORT;
}

template <>
inline hid_t
get_datatype<unsigned short>()
{
    return H5T_NATIVE_USHORT;
}

template <>
inline hid_t
get_datatype<int>()
{
    return H5T_NATIVE_INT;
}

template <>
inline hid_t
get_datatype<unsigned int>()
{
    return H5T_NATIVE_UINT;
}

template <>
inline hid_t
get_datatype<long>()
{
    return H5T_NATIVE_LONG;
}

template <>
inline hid_t
get_datatype<unsigned long>()
{
    return H5T_NATIVE_ULONG;
}

template <>
inline hid_t
get_datatype<long long>()
{
    return H5T_NATIVE_LLONG;
}

template <>
inline hid_t
get_datatype<unsigned long long>()
{
    return H5T_NATIVE_ULLONG;
}

template <>
inline hid_t
get_datatype<float>()
{
    return H5T_NATIVE_FLOAT;
}

template <>
inline hid_t
get_datatype<double>()
{
    return H5T_NATIVE_DOUBLE;
}

template <>
inline hid_t
get_datatype<bool>()
{
    return H5T_NATIVE_INT;
}

template <>
inline hid_t
get_datatype<std::string>()
{
    static std::once_flag reg;
    static hid_t dtype;
    std::call_once(reg, [] {
        dtype = H5Tcopy(H5T_C_S1);
        H5Tset_size(dtype, H5T_VARIABLE);
    });
    return dtype;
}

} // namespace hdf5

/// Class for interaction with HDF5 files
class HDF5File {
    class Dataset;

    class Group {
        Group(hid_t id) : id(id) {}

    public:
        ~Group();

        const hid_t id;

        Group create_group(const std::string & name);

        Group open_group(const std::string & name);

        bool has_attribute(const std::string & name) const;

        bool has_dataset(const std::string & name) const;

        template <typename T>
        void write_attribute(const std::string & name, const T & value);

        template <typename T>
        T read_attribute(const std::string & name) const;

        template <typename T>
        void write_dataset(const std::string & name, const T & data);

        template <typename T>
        void write_dataset(const std::string & name, Int n, const T data[]);

        template <typename T>
        T read_dataset(const std::string & name) const;

        template <typename T>
        void read_dataset(const std::string & name, T & data);

        template <typename T>
        void read_dataset(const std::string & name, Int n, T data[]);

    public:
        static Group
        open(hid_t parent_id, const std::string & name)
        {
            auto id = H5Gopen2(parent_id, name.c_str(), H5P_DEFAULT);
            if (id < 0)
                throw Exception("Failed to open group '{}'", name);
            return Group(id);
        }

        static Group
        create(hid_t parent_id, const std::string & name)
        {
            if (name == "/") {
                return Group::open(parent_id, name);
            }
            else if (H5Lexists(parent_id, name.c_str(), H5P_DEFAULT) > 0) {
                return Group::open(parent_id, name);
            }
            else {
                auto id =
                    H5Gcreate2(parent_id, name.c_str(), H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
                if (id < 0)
                    throw Exception("Failed to create group '{}'", name);
                return Group(id);
            }
        }
    };

    class Dataspace {
        Dataspace(hid_t id) : id(id) {}

    public:
        ~Dataspace() { H5Sclose(this->id); }

        int
        get_simple_extent_ndims() const
        {
            return H5Sget_simple_extent_ndims(this->id);
        }

        std::vector<hsize_t>
        get_simple_extent_dims() const
        {
            auto n = get_simple_extent_ndims();
            std::vector<hsize_t> dims(n);
            if (H5Sget_simple_extent_dims(this->id, dims.data(), NULL) < 0)
                throw Exception("Failed to obtain dataspace dimension");
            else
                return dims;
        }

        const hid_t id;

    public:
        template <typename T>
        static Dataspace
        create()
        {
            const hsize_t dims[1] = { 1 };
            const hsize_t max_dims[1] = { 1 };
            auto id = H5Screate_simple(1, dims, max_dims);
            if (id == H5I_INVALID_HID)
                throw Exception("Failed to create dataspace");
            return Dataspace(id);
        }

        static Dataspace
        create(hsize_t n)
        {
            const hsize_t dims[1] = { n };
            const hsize_t max_dims[1] = { n };
            auto id = H5Screate_simple(1, dims, max_dims);
            if (id == H5I_INVALID_HID)
                throw Exception("Failed to create dataspace");
            return Dataspace(id);
        }

        static Dataspace
        create_scalar()
        {
            auto id = H5Screate(H5S_SCALAR);
            if (id == H5I_INVALID_HID)
                throw Exception("Failed to create scalar dataspace");
            return Dataspace(id);
        }

        friend class Dataset;
    };

    class Dataset {
        Dataset(hid_t id) : id(id) {}

    public:
        ~Dataset() { H5Dclose(this->id); }

        template <typename T>
        void read(T & data) const;

        template <typename T, typename A>
        void read(std::vector<T, A> & data) const;

        template <typename T>
        void read(Int n, T data[]);

        template <typename T>
        void write(const T & data);

        template <typename T, typename A>
        void write(const std::vector<T, A> & data);

        template <typename T>
        void write(Int n, const T data[]);

        Dataspace
        get_space() const
        {
            return Dataspace(H5Dget_space(this->id));
        }

        const hid_t id;

        template <typename T>
        static Dataset
        create(hid_t parent_id, const std::string & name, const Dataspace & dspace)
        {
            auto id = H5Dcreate2(parent_id,
                                 name.c_str(),
                                 hdf5::get_datatype<T>(),
                                 dspace.id,
                                 H5P_DEFAULT,
                                 H5P_DEFAULT,
                                 H5P_DEFAULT);
            return Dataset(id);
        }

        static Dataset
        open(hid_t parent_id, const std::string & name)
        {
            auto id = H5Dopen2(parent_id, name.c_str(), H5P_DEFAULT);
            if (id == H5I_INVALID_HID)
                throw Exception("Unable to open dataset '{}'", name);
            return Dataset(id);
        }
    };

    class Attribute {
        Attribute(hid_t id) : id(id) {}

    public:
        ~Attribute() { H5Aclose(this->id); }

        const hid_t id;

        template <typename T>
        static Attribute
        create(hid_t parent_id, const std::string & name, const Dataspace & dspace)
        {
            auto id = H5Acreate2(parent_id,
                                 name.c_str(),
                                 hdf5::get_datatype<T>(),
                                 dspace.id,
                                 H5P_DEFAULT,
                                 H5P_DEFAULT);
            if (id == H5I_INVALID_HID)
                throw Exception("Failed to create attribute '{}'", name);
            return Attribute(id);
        }

        template <typename T>
        static Attribute
        open(hid_t parent_id, const std::string & name)
        {
            if (!H5Aexists(parent_id, name.c_str()))
                throw Exception("Attribute {} does not exist", name);

            auto id = H5Aopen(parent_id, name.c_str(), H5P_DEFAULT);
            if (id == H5I_INVALID_HID)
                throw Exception("Unable to open attribute '{}'", name);

            return Attribute(id);
        }

        template <typename T>
        void read(T & data) const;

        template <typename T>
        void write(const T & data);
    };

public:
    HDF5File(const fs::path & file_name, FileAccess faccess);
    ~HDF5File();

    std::string get_file_name() const;

    std::string get_file_path() const;

    bool has_attribute(const std::string & name) const;

    bool has_dataset(const std::string & name) const;

    template <typename T>
    void write_dataset(const std::string & name, const T & data);

    template <typename T>
    void write_dataset(const std::string & name, Int n, const T data[]);

    template <typename T>
    T read_dataset(const std::string & name) const;

    template <typename T>
    void read_dataset(const std::string & name, Int n, T data[]) const;

    Group create_group(const std::string & name) const;

    Group open_group(const std::string & name) const;

    template <typename T>
    void write_attribute(const std::string & name, const T & value);

    template <typename T>
    T read_attribute(const std::string & name) const;

private:
    hid_t id;
    fs::path file_name;
};

// Group

inline HDF5File::Group::~Group()
{
    H5Gclose(this->id);
}

inline bool
HDF5File::Group::has_attribute(const std::string & name) const
{
    auto res = H5Aexists(this->id, name.c_str());
    if (res < 0)
        throw Exception("Failed to check attribute");
    return res > 0;
}

inline bool
HDF5File::Group::has_dataset(const std::string & name) const
{
    auto res = H5Lexists(this->id, name.c_str(), H5P_DEFAULT);
    if (res < 0)
        throw Exception("Failed to check dataset");
    return res > 0;
}

inline HDF5File::Group
HDF5File::Group::create_group(const std::string & name)
{
    return Group::create(this->id, name);
}

inline HDF5File::Group
HDF5File::Group::open_group(const std::string & name)
{
    return Group::open(this->id, name);
}

template <typename T>
inline void
HDF5File::Group::write_attribute(const std::string & name, const T & value)
{
    if constexpr (StdVector<T>) {
        throw Exception("Vector-valued attributes are not supported yet");
    }
    else {
        auto dataspace = Dataspace::create_scalar();
        auto attribute = Attribute::create<T>(this->id, name, dataspace);
        attribute.template write<T>(value);
    }
}

template <typename T>
inline T
HDF5File::Group::read_attribute(const std::string & name) const
{
    if constexpr (StdVector<T>) {
        throw Exception("Vector-valued attributes are not supported yet");
    }
    else {
        T value;
        auto attribute = Attribute::open<T>(this->id, name);
        attribute.template read<T>(value);
        return value;
    }
}

template <typename T>
inline void
HDF5File::Group::write_dataset(const std::string & name, const T & data)
{
    if constexpr (StdVector<T>) {
        using V = typename T::value_type;
        auto dataspace = Dataspace::create(data.size());
        auto dataset = Dataset::create<V>(this->id, name, dataspace);
        dataset.template write<V, std::allocator<V>>(data);
    }
    else {
        auto dataspace = Dataspace::create<T>();
        auto dataset = Dataset::create<T>(this->id, name, dataspace);
        dataset.template write<T>(data);
    }
}

template <typename T>
inline void
HDF5File::Group::write_dataset(const std::string & name, Int n, const T data[])
{
    auto dataspace = Dataspace::create(n);
    auto dataset = Dataset::create<T>(this->id, name, dataspace);
    dataset.template write<T>(n, data);
}

template <typename T>
inline T
HDF5File::Group::read_dataset(const std::string & name) const
{
    T data;
    auto dataset = Dataset::open(this->id, name);
    if constexpr (StdVector<T>) {
        using V = typename T::value_type;
        dataset.template read<V, std::allocator<V>>(data);
    }
    else {
        dataset.template read<T>(data);
    }
    return data;
}

template <typename T>
inline void
HDF5File::Group::read_dataset(const std::string & name, T & data)
{
    auto dataset = Dataset::open(this->id, name);
    if constexpr (StdVector<T>) {
        using V = typename T::value_type;
        dataset.template read<V, std::allocator<V>>(data);
    }
    else {
        dataset.template read<T>(data);
    }
}

template <typename T>
inline void
HDF5File::Group::read_dataset(const std::string & name, Int n, T data[])
{
    auto dataset = Dataset::open(this->id, name);
    dataset.template read<T>(n, data);
}

// Dataspace

template <>
inline HDF5File::Dataspace
HDF5File::Dataspace::create<std::string>()
{
    return Dataspace::create_scalar();
}

// Dataset

template <typename T>
inline void
HDF5File::Dataset::read(T & data) const
{
    auto res = H5Dread(this->id, hdf5::get_datatype<T>(), H5S_ALL, H5S_ALL, H5P_DEFAULT, &data);
    if (res < 0)
        throw Exception("Failed reading dataset");
}

template <>
inline void
HDF5File::Dataset::read(std::string & data) const
{
    auto dtype = H5Dget_type(this->id);
    if (H5Tis_variable_str(dtype) > 0) {
        char * c_str = nullptr;
        auto res = H5Dread(this->id, dtype, H5S_ALL, H5S_ALL, H5P_DEFAULT, &c_str);
        if (res < 0)
            throw Exception("Failed reading dataset");
        data = std::string(c_str);
        H5free_memory(c_str);
    }
    else {
        size_t size = H5Tget_size(dtype);
        data.resize(size);
        auto res = H5Dread(this->id, dtype, H5S_ALL, H5S_ALL, H5P_DEFAULT, data.data());
        if (res < 0)
            throw Exception("Error reading dataset");
    }
    H5Tclose(dtype);
}

template <typename T, typename A>
inline void
HDF5File::Dataset::read(std::vector<T, A> & data) const
{
    auto dataspace = get_space();
    auto dims = dataspace.get_simple_extent_dims();
    auto n = std::accumulate(dims.begin(), dims.end(), 1, std::multiplies());
    data.resize(n);
    auto res =
        H5Dread(this->id, hdf5::get_datatype<T>(), H5S_ALL, H5S_ALL, H5P_DEFAULT, data.data());
    if (res < 0)
        throw Exception("Failed reading dataset");
}

template <typename T>
inline void
HDF5File::Dataset::read(Int n, T data[])
{
    auto res = H5Dread(this->id, hdf5::get_datatype<T>(), H5S_ALL, H5S_ALL, H5P_DEFAULT, data);
    if (res < 0)
        throw Exception("Error reading dataset");
}

template <typename T>
inline void
HDF5File::Dataset::write(const T & data)
{
    auto res = H5Dwrite(this->id, hdf5::get_datatype<T>(), H5S_ALL, H5S_ALL, H5P_DEFAULT, &data);
    if (res < 0)
        throw Exception("Error writing dataset");
}

template <>
inline void
HDF5File::Dataset::write(const std::string & data)
{
    const char * c_str = data.c_str();
    auto res = H5Dwrite(this->id,
                        hdf5::get_datatype<std::string>(),
                        H5S_ALL,
                        H5S_ALL,
                        H5P_DEFAULT,
                        &c_str);
    if (res < 0)
        throw Exception("Error writing dataset");
}

template <typename T, typename A>
inline void
HDF5File::Dataset::write(const std::vector<T, A> & data)
{
    auto res =
        H5Dwrite(this->id, hdf5::get_datatype<T>(), H5S_ALL, H5S_ALL, H5P_DEFAULT, data.data());
    if (res < 0)
        throw Exception("Error writing dataset");
}

template <typename T>
inline void
HDF5File::Dataset::write(Int n, const T data[])
{
    auto res = H5Dwrite(this->id, hdf5::get_datatype<T>(), H5S_ALL, H5S_ALL, H5P_DEFAULT, data);
    if (res < 0)
        throw Exception("Error writing dataset");
}

// Attribute

template <typename T>
inline void
HDF5File::Attribute::read(T & data) const
{
    auto res = H5Aread(this->id, hdf5::get_datatype<T>(), &data);
    if (res < 0)
        throw Exception("Error reading attribute");
}

template <>
inline void
HDF5File::Attribute::read(std::string & data) const
{
    auto dtype = H5Aget_type(this->id);
    if (H5Tis_variable_str(dtype) > 0) {
        char * str;
        auto res = H5Aread(this->id, dtype, &str);
        if (res < 0)
            throw Exception("Error reading attribute");
        data = std::string(str);
        H5free_memory(str);
    }
    else {
        size_t size = H5Tget_size(dtype);
        data.resize(size);
        auto res = H5Aread(this->id, dtype, data.data());
        if (res < 0)
            throw Exception("Error reading attribute");
    }
    H5Tclose(dtype);
}

template <typename T>
inline void
HDF5File::Attribute::write(const T & data)
{
    auto res = H5Awrite(this->id, hdf5::get_datatype<T>(), &data);
    if (res < 0)
        throw Exception("Error writing attribute");
}

template <>
inline void
HDF5File::Attribute::write(const std::string & data)
{
    auto cstr = data.data();
    auto res = H5Awrite(this->id, hdf5::get_datatype<std::string>(), &cstr);
    if (res < 0)
        throw Exception("Error writing attribute");
}

// HDF5File

inline HDF5File::Group
HDF5File::create_group(const std::string & name) const
{
    return Group::create(this->id, name);
}

inline HDF5File::Group
HDF5File::open_group(const std::string & name) const
{
    return Group::open(this->id, name);
}

inline bool
HDF5File::has_attribute(const std::string & name) const
{
    auto res = H5Aexists(this->id, name.c_str());
    if (res < 0)
        throw Exception("Failed to check attribute");
    return res > 0;
}

inline bool
HDF5File::has_dataset(const std::string & name) const
{
    auto res = H5Lexists(this->id, name.c_str(), H5P_DEFAULT);
    if (res < 0)
        throw Exception("Failed to check dataset");
    return res > 0;
}

template <typename T>
inline void
HDF5File::write_dataset(const std::string & name, const T & data)
{
    if constexpr (StdVector<T>) {
        using V = typename T::value_type;
        auto dataspace = Dataspace::create(data.size());
        auto dataset = Dataset::create<V>(this->id, name, dataspace);
        dataset.template write<V, std::allocator<V>>(data);
    }
    else {
        auto dataspace = Dataspace::create<T>();
        auto dataset = Dataset::create<T>(this->id, name, dataspace);
        dataset.template write<T>(data);
    }
}

template <typename T>
inline void
HDF5File::write_dataset(const std::string & name, Int n, const T data[])
{
    auto dataspace = Dataspace::create(n);
    auto dataset = Dataset::create<T>(this->id, name, dataspace);
    dataset.template write<T>(n, data);
}

template <typename T>
inline T
HDF5File::read_dataset(const std::string & name) const
{
    T data;
    auto dataset = Dataset::open(this->id, name);
    if constexpr (StdVector<T>) {
        using V = typename T::value_type;
        dataset.template read<V, std::allocator<V>>(data);
    }
    else {
        dataset.template read<T>(data);
    }
    return data;
}

template <typename T>
inline void
HDF5File::read_dataset(const std::string & name, Int n, T data[]) const
{
    auto dataset = Dataset::open(this->id, name);
    dataset.template read<T>(n, data);
}

template <typename T>
inline void
HDF5File::write_attribute(const std::string & name, const T & value)
{
    if constexpr (StdVector<T>) {
        throw Exception("Vector-valued attributes are not supported yet");
    }
    else {
        auto dataspace = Dataspace::create_scalar();
        auto attribute = Attribute::create<T>(this->id, name, dataspace);
        attribute.template write<T>(value);
    }
}

template <typename T>
inline T
HDF5File::read_attribute(const std::string & name) const
{
    if constexpr (StdVector<T>) {
        throw Exception("Vector-valued attributes are not supported yet");
    }
    else {
        T value;
        auto attribute = Attribute::open<T>(this->id, name);
        attribute.template read<T>(value);
        return value;
    }
}

} // namespace godzilla
