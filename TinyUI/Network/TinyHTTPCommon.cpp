#include "../stdafx.h"
#include "TinyHTTPCommon.h"

namespace TinyUI
{
	namespace Network
	{
		IOBuffer::IOBuffer()
			: m_data(NULL)
		{
		}

		IOBuffer::IOBuffer(INT size)
			: m_size(size)
		{
			m_data = new char[size];
		}

		IOBuffer::IOBuffer(CHAR* data, INT size)
			: m_data(data),
			m_size(size)
		{
		}

		IOBuffer::~IOBuffer()
		{
			SAFE_DELETE_ARRAY(m_data);
		}

		CHAR* IOBuffer::data() const
		{
			return m_data;
		}
		INT	IOBuffer::size() const
		{
			return m_size;
		}
		//////////////////////////////////////////////////////////////////////////
		StringIOBuffer::StringIOBuffer(const std::string& s)
			: IOBuffer(static_cast<CHAR*>(NULL), 0),
			m_value(std::move(s))
		{
			m_data = &m_value[0];
			m_size = m_value.size();
		}
		StringIOBuffer::StringIOBuffer(std::unique_ptr<std::string> s)
			: IOBuffer(static_cast<char*>(NULL), 0)
		{
			m_value.swap(*s.get());
			m_data = &m_value[0];
			m_size = m_value.size();
		}

		INT StringIOBuffer::size() const
		{
			return static_cast<int>(m_value.size());
		}
		StringIOBuffer::~StringIOBuffer()
		{
			m_data = NULL;
		}
		//////////////////////////////////////////////////////////////////////////
		DrainableIOBuffer::DrainableIOBuffer(IOBuffer* base)
			: IOBuffer(base->data(), base->size()),
			m_base(base),
			m_bytes(0)
		{

		}

		void DrainableIOBuffer::SetConsume(INT bytes)
		{
			SetOffset(m_bytes + bytes);
		}

		INT DrainableIOBuffer::Remaining() const
		{
			return m_size - m_bytes;
		}

		INT DrainableIOBuffer::Consume() const
		{
			return m_bytes;
		}

		void DrainableIOBuffer::SetOffset(INT bytes)
		{
			m_bytes = bytes;
			m_data = m_base->data() + m_bytes;
		}

		DrainableIOBuffer::~DrainableIOBuffer()
		{
			m_data = NULL;
		}
	}
}