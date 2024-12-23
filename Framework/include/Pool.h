﻿//-----------------------------------------------------------------------------
// File : Pool.h
// Desc : Item Pool.
// Copyright(c) Pocol. All right reserved.
//-----------------------------------------------------------------------------
#pragma once

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <cstdint>
#include <mutex>
#include <cassert>
#include <functional>

///////////////////////////////////////////////////////////////////////////////
// Pool class
///////////////////////////////////////////////////////////////////////////////
template<typename T>
class Pool
{
	//=========================================================================
	// list of friend classes and methods.
	//=========================================================================
	/* NOTHING */

public:
	//=========================================================================
	// public variablse.
	//=========================================================================

	//-------------------------------------------------------------------------
	//! @brief      コンストラクタです.
	//-------------------------------------------------------------------------
	Pool()
		: m_pBuffer(nullptr)
		, m_pActive(nullptr)
		, m_pFree(nullptr)
		, m_Capacity(0)
		, m_Count(0)
	{ /* DO_NOTHING */
	}

	//-------------------------------------------------------------------------
	//! @brief      デストラクタです.
	//-------------------------------------------------------------------------
	~Pool()
	{
		Term();
	}

	//-------------------------------------------------------------------------
	//! @brief      初期化処理を行います.
	//!
	//! @param[in]      count       確保するアイテム数です.
	//! @retval true    初期化に成功.
	//! @retval false   初期化に失敗.
	//-------------------------------------------------------------------------
	bool Init(uint32_t count)
	{
		std::lock_guard<std::mutex> guard(m_Mutex);

		m_pBuffer = static_cast<uint8_t*>(malloc(sizeof(Item) * (count + 2)));
		if (m_pBuffer == nullptr)
		{
			return false;
		}

		m_Capacity = count;

		// インデックスを振る.
		for (auto i = 2u, j = 0u; i < m_Capacity + 2; ++i, ++j)
		{
			auto item = GetItem(i);
			item->m_Index = j;
		}

		m_pActive = GetItem(0);
		m_pActive->m_pPrev = m_pActive->m_pNext = m_pActive;
		m_pActive->m_Index = uint32_t(-1);

		m_pFree = GetItem(1);
		m_pFree->m_Index = uint32_t(-2);

		for (auto i = 1u; i < m_Capacity + 2; ++i)
		{
			GetItem(i)->m_pPrev = nullptr;
			GetItem(i)->m_pNext = GetItem(i + 1);
		}

		GetItem(m_Capacity + 1)->m_pPrev = m_pFree;

		m_Count = 0;

		return true;
	}

	//-------------------------------------------------------------------------
	//! @brief      終了処理を行います.
	//-------------------------------------------------------------------------
	void Term()
	{
		std::lock_guard<std::mutex> guard(m_Mutex);

		if (m_pBuffer)
		{
			free(m_pBuffer);
			m_pBuffer = nullptr;
		}

		m_pActive = nullptr;
		m_pFree = nullptr;
		m_Capacity = 0;
		m_Count = 0;
	}

	//-------------------------------------------------------------------------
	//! @brief      アイテムを確保します.
	//!
	//! @param[in]      func        ユーザによる初期化処理です.
	//! @return     確保したアイテムへのポインタ. 確保に失敗した場合は nullptr が返却されます.
	//-------------------------------------------------------------------------
	T* Alloc(std::function<void(uint32_t, T*)> func = nullptr)
	{
		std::lock_guard<std::mutex> guard(m_Mutex);

		if (m_pFree->m_pNext == m_pFree || m_Count + 1 > m_Capacity)
		{
			return nullptr;
		}

		auto item = m_pFree->m_pNext;
		m_pFree->m_pNext = item->m_pNext;

		item->m_pPrev = m_pActive->m_pPrev;
		item->m_pNext = m_pActive;
		item->m_pPrev->m_pNext = item->m_pNext->m_pPrev = item;

		m_Count++;

		// メモリ割り当て.
		auto val = new ((void*)item) T();

		// 初期化の必要があれば呼び出す.
		if (func != nullptr)
		{
			func(item->m_Index, val);
		}

		return val;
	}

	//-------------------------------------------------------------------------
	//! @brief      アイテムを解放します.
	//!
	//! @param[in]      pValue      解放するアイテムへのポインタ.
	//-------------------------------------------------------------------------
	void Free(T* pValue)
	{
		if (pValue == nullptr)
		{
			return;
		}

		std::lock_guard<std::mutex> guard(m_Mutex);

		auto item = reinterpret_cast<Item*>(pValue);

		item->m_pPrev->m_pNext = item->m_pNext;
		item->m_pNext->m_pPrev = item->m_pPrev;

		item->m_pPrev = nullptr;
		item->m_pNext = m_pFree->m_pNext;

		m_pFree->m_pNext = item;
		m_Count--;
	}

	//-------------------------------------------------------------------------
	//! @brief      総アイテム数を取得します.
	//!
	//! @return     総アイテム数を返却します.
	//-------------------------------------------------------------------------
	uint32_t GetSize() const
	{
		return m_Capacity;
	}

	//-------------------------------------------------------------------------
	//! @brief      使用中のアイテム数を取得します.
	//!
	//! @return     使用中のアイテム数を返却します.
	//-------------------------------------------------------------------------
	uint32_t GetUsedCount() const
	{
		return m_Count;
	}

	//-------------------------------------------------------------------------
	//! @brief      利用可能なアイテム数を取得します.
	//!
	//! @return     利用可能なアイテム数を返却します.
	//-------------------------------------------------------------------------
	uint32_t GetAvailableCount() const
	{
		return m_Capacity - m_Count;
	}

private:
	///////////////////////////////////////////////////////////////////////////
	// Item structure
	///////////////////////////////////////////////////////////////////////////
	struct Item
	{
		T           m_Value;        //!< 値です.
		uint32_t    m_Index;        //!< インデックスです.
		Item* m_pNext;        //!< 次のアイテムへのポインタ.
		Item* m_pPrev;        //!< 前のアイテムへのポインタ.

		Item()
			: m_Value()
			, m_Index(0)
			, m_pNext(nullptr)
			, m_pPrev(nullptr)
		{ /* DO_NOTHING */
		}

		~Item()
		{ /* DO_NOTHING */
		}
	};

	//=========================================================================
	// private variables.
	//=========================================================================
	uint8_t* m_pBuffer;      //!< バッファです.
	Item* m_pActive;      //!< アクティブアイテムの先頭です.
	Item* m_pFree;        //!< フリーアイテムの先頭です.
	uint32_t    m_Capacity;     //!< 総アイテム数です.
	uint32_t    m_Count;        //!< 確保したアイテム数です.
	std::mutex  m_Mutex;        //!< ミューテックスです.

	//=========================================================================
	// private methods.
	//=========================================================================

	//-------------------------------------------------------------------------
	//! @brief      アイテムを取得します.
	//!
	//! @param[in]      index       取得するアイテムのインデックス.
	//! @return     アイテムへのポインタを返却します.
	//-------------------------------------------------------------------------
	Item* GetItem(uint32_t index)
	{
		assert(0 <= index && index <= m_Capacity + 2);
		return reinterpret_cast<Item*>(m_pBuffer + sizeof(Item) * index);
	}

	//-------------------------------------------------------------------------
	//! @brief      アイテムにメモリを割り当てます.
	//!
	//! @param[in]      index       取得するアイテムのインデックス.
	//! @return     アイテムへのポインタを返却します.
	//-------------------------------------------------------------------------
	Item* AssignItem(uint32_t index)
	{
		assert(0 <= index && index <= m_Capacity + 2);
		auto buf = (m_pBuffer + sizeof(Item) * index);
		return new (buf) Item;
	}

	Pool(const Pool&) = delete;
	void operator = (const Pool&) = delete;
};
