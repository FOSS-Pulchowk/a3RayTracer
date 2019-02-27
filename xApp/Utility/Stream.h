#pragma once
#include "Common/Core.h"

//
// DECLARATION
//

namespace a3 {

	struct stream
	{
	private:
		u8* m_Begin;
		u8* m_End;
		u64 m_Size;
		u8* m_Current;

	public:
		stream();
		void SetWorkingBuffer(u8* buffer, u64 length);
		void ReleaseWorkingBuffer();
		u8* GetWorkingBufferBegin();
		u8* GetWorkingBufferEnd(); // NOTE(Zero): Return 1 past the end
		u64 GetWorkingBufferLength();
		u8* GetWorkingBufferPointer();
		u8 GetCurrentPointerValue();
		u8* PopCurrentPointer();
		u8 PopCurrentPointerValue();
		stream& Seek(u64 position);
		stream& MoveForward(u64 step);
		stream& MoveBackward(u64 step);
		stream& MoveHome();
		stream& MoveEnd();
		stream& MoveToNextLine();
		stream& MoveToPreviousLine();
		stream& MoveCharacterForward();
		stream& MoveCharacterBackward();
		stream& MoveWordForward();
		stream& MoveWordBackward();
		stream& MoveForwardTo(u8 c);
		stream& MoveBackwardTo(u8 c);
		stream& MoveForwardPass(u8 c);
		stream& MoveBackwardPass(u8 c);
		stream& MoveLineForwardTo(u8 c);
		stream& MoveLineForwardPass(u8 c);
		stream& MoveLineBackwardTo(u8 c);
		stream& MoveLineBackwardPass(u8 c);
		i32 Count(u8 c, u8 end = 0);
		i32 Count(u8 c, u8* end);
		i32 CountInLine(u8 c, u8 end = 0);
		i32 CountInLine(u8 c, u8* end);
		i32 FindWordInLine(s8 word);
		i32 FindWord(s8 word);
	};

}


//
// DEFINATION
//

#ifdef A3_IMPLEMENT_STREAM

namespace a3 {

	stream::stream() :
		m_Begin(0), m_End(0), m_Size(0), m_Current(0)
	{
	}

	void stream::SetWorkingBuffer(u8* buffer, u64 length)
	{
		m_Begin = buffer;
		m_Size = length;
		m_Current = m_Begin;
		m_End = buffer + m_Size;
	}

	void stream::ReleaseWorkingBuffer()
	{
		m_Begin = 0;
		m_Size = 0;
		m_Current = 0;
		m_End = 0;
	}

	u8* stream::GetWorkingBufferBegin()
	{
		return m_Begin;
	}

	u8* stream::GetWorkingBufferEnd()
	{
		return m_End + 1;
	}

	u64 stream::GetWorkingBufferLength()
	{
		return m_Size;
	}

	u8* stream::GetWorkingBufferPointer()
	{
		return m_Current;
	}

	u8 stream::GetCurrentPointerValue()
	{
		return *m_Current;
	}

	u8* stream::PopCurrentPointer()
	{
		return m_Current++;
	}

	u8 stream::PopCurrentPointerValue()
	{
		return *((++m_Current) - 1);
	}

	stream& stream::Seek(u64 position)
	{
		a3Assert(m_Begin);
		if (position < m_Size)
			m_Current = m_Begin + position;
		return *this;
	}

	stream& stream::MoveForward(u64 step)
	{
		a3Assert(m_Begin);
		if (m_Current + step < m_End)
			m_Current += step;
		else
			m_Current = m_End;
		return *this;
	}

	stream& stream::MoveBackward(u64 step)
	{
		a3Assert(m_Begin);
		if (m_Current - step > m_Begin)
			m_Current -= step;
		else
			m_Current = m_Begin;
		return *this;
	}

	stream& stream::MoveHome()
	{
		MoveBackwardTo('\n');
		MoveCharacterForward();
		return *this;
	}

	stream& stream::MoveEnd()
	{
		MoveForwardTo('\n');
		return *this;
	}

	stream& stream::MoveToNextLine()
	{
		MoveEnd();
		MoveCharacterForward();
		return *this;
	}

	stream& stream::MoveToPreviousLine()
	{
		MoveHome();
		MoveCharacterBackward();
		MoveHome();
		return *this;
	}

	stream& stream::MoveCharacterForward()
	{
		if (m_Current != m_End) m_Current++;
		return *this;
	}

	stream& stream::MoveCharacterBackward()
	{
		if (m_Current != m_Begin) m_Current--;
		return *this;
	}

	stream& stream::MoveWordForward()
	{
		MoveForwardPass(' ');
		return *this;
	}

	stream& stream::MoveWordBackward()
	{
		MoveBackwardTo(' ');
		MoveCharacterForward();
		return *this;
	}

	stream& stream::MoveForwardTo(u8 c)
	{
		while (*m_Current != c)
		{
			if (m_Current == m_End) break;
			m_Current++;
		}
		return *this;
	}

	stream& stream::MoveBackwardTo(u8 c)
	{
		while (*m_Current != c)
		{
			if (m_Current == m_Begin) break;
			m_Current--;
		}
		return *this;
	}

	stream& stream::MoveForwardPass(u8 c)
	{
		MoveForwardTo(c);
		MoveCharacterForward();
		return *this;
	}

	stream& stream::MoveBackwardPass(u8 c)
	{
		MoveBackwardTo(c);
		MoveCharacterBackward();
		return *this;
	}

	stream & stream::MoveLineForwardTo(u8 c)
	{
		while (*m_Current != c && *m_Current != '\n')
		{
			if (m_Current == m_End) break;
			m_Current++;
		}
		return *this;
	}

	stream & stream::MoveLineForwardPass(u8 c)
	{
		MoveForwardTo(c);
		MoveCharacterForward();
		return *this;
	}

	stream & stream::MoveLineBackwardTo(u8 c)
	{
		while (*m_Current != c && *(m_Current-1) != '\n')
		{
			if (m_Current == m_End) break;
			m_Current--;
		}
		return *this;
	}

	stream & stream::MoveLineBackwardPass(u8 c)
	{
		MoveLineBackwardTo(c);
		MoveCharacterBackward();
		return *this;
	}

	i32 stream::Count(u8 c, u8 end)
	{
		i32 count = 0;
		u8* scan = m_Current;
		while (*scan != end && scan != m_End)
		{
			if (*scan == c) count++;
			scan++;
		}
		return count;
	}

	i32 stream::Count(u8 c, u8 * end)
	{
		i32 count = 0;
		u8* scan = m_Current;
		while (*scan != *end && scan != m_End + 1 && scan != end)
		{
			if (*scan == c) count++;
			scan++;
		}
		return count;
	}

	inline i32 stream::CountInLine(u8 c, u8 end)
	{
		i32 count = 0;
		u8* scan = m_Current;
		while ((*scan != end) && (scan != m_End + 1) && (*scan != '\n'))
		{
			if (*scan == c) count++;
			scan++;
		}
		return count;
	}

	inline i32 stream::CountInLine(u8 c, u8 * end)
	{
		i32 count = 0;
		u8* scan = m_Current;
		while ((*scan != *end) && (scan != m_End + 1) && (scan != end) && (*scan != '\n'))
		{
			if (*scan == c) count++;
			scan++;
		}
		return count;
	}

	i32 stream::FindWordInLine(s8  word)
	{
		i32 pos = -1;
		u8* scan = m_Current;
		u32 index = 0;
		u32 count = 0;
		while ((*scan != '\n') && (scan != m_End + 1))
		{
			if (word[index] == 0) break;
			if (*scan == (u8)word[index])
			{
				if (index == 0) pos = count;
				index++;
			}
			else
			{
				pos = -1;
				index = 0;
			}
			scan++;
			count++;
		}
		return pos;
	}

	i32 stream::FindWord(s8  word)
	{
		i32 pos = -1;
		u8* scan = m_Current;
		u32 index = 0;
		u32 count = 0;
		while (scan != m_End + 1)
		{
			if (word[index] == 0) break;
			if (*scan == (u8)word[index])
			{
				if (index == 0) pos = count;
				index++;
			}
			else
			{
				pos = -1;
				index = 0;
			}
			scan++;
			count++;
		}
		return pos;
	}

}

#endif