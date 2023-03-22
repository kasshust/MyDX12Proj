//-----------------------------------------------------------------------------
// File : RootSignature.h
// Desc : RootSignature Wrapper.
// Copyright(c) Pocol. All right reserved.
//-----------------------------------------------------------------------------
#pragma once

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <ComPtr.h>
#include <d3d12.h>
#include <vector>


///////////////////////////////////////////////////////////////////////////
// ShaderState enum
///////////////////////////////////////////////////////////////////////////
enum ShaderStage
{
    ALL = 0,     //!< �S�X�e�[�W.
    VS  = 1,     //!< ���_�V�F�[�_.
    HS  = 2,     //!< �n���V�F�[�_.
    DS  = 3,     //!< �h���C���V�F�[�_.
    GS  = 4,     //!< �W�I���g���V�F�[�_.
    PS  = 5,     //!< �s�N�Z���V�F�[�_.
};

///////////////////////////////////////////////////////////////////////////
// SamplerState enum
///////////////////////////////////////////////////////////////////////////
enum SamplerState
{
    PointWrap,          //!< �|�C���g�T���v�����O - �J��Ԃ�.
    PointClamp,         //!< �|�C���g�T���v�����O - �N�����v.
    LinearWrap,         //!< �g���C���j�A�T���v�����O - �J��Ԃ�.
    LinearClamp,        //!< �g���C���j�A�T���v�����O - �N�����v.
    AnisotropicWrap,    //!< �ٕ����T���v�����O - �J��Ԃ�.
    AnisotropicClamp,   //!< �ٕ����T���v�����O - �N�����v.
};


///////////////////////////////////////////////////////////////////////////////
// RootSignature class
///////////////////////////////////////////////////////////////////////////////
class RootSignature
{
    //=========================================================================
    // list of friend classes and methods.
    //=========================================================================
    /* NOTHING */

public:

    ///////////////////////////////////////////////////////////////////////////
    // Desc class
    ///////////////////////////////////////////////////////////////////////////
    class Desc
    {
    public:
        Desc ();
        ~Desc();
        Desc& Begin(int count);
        Desc& SetCBV(ShaderStage stage, int index, uint32_t reg);
        Desc& SetSRV(ShaderStage stage, int index, uint32_t reg);
        Desc& SetUAV(ShaderStage stage, int index, uint32_t reg);
        Desc& SetSmp(ShaderStage stage, int index, uint32_t reg);
        Desc& AddStaticSmp(ShaderStage stage, uint32_t reg, SamplerState state);
        Desc& AllowIL();
        Desc& AllowSO();
        Desc& End();
        const D3D12_ROOT_SIGNATURE_DESC* GetDesc() const;

    private:
        std::vector<D3D12_DESCRIPTOR_RANGE>     m_Ranges;
        std::vector<D3D12_STATIC_SAMPLER_DESC>  m_Samplers;
        std::vector<D3D12_ROOT_PARAMETER>       m_Params;
        D3D12_ROOT_SIGNATURE_DESC               m_Desc;
        bool                                    m_DenyStage[5];
        uint32_t                                m_Flags;

        void CheckStage(ShaderStage stage);
        void SetParam  (ShaderStage, int index, uint32_t reg, D3D12_DESCRIPTOR_RANGE_TYPE type);
    };

    //=========================================================================
    // public methods.
    //=========================================================================
    RootSignature();
    ~RootSignature();
    bool Init(ID3D12Device* pDevice, const D3D12_ROOT_SIGNATURE_DESC* pDesc);
    void Term();
    ID3D12RootSignature* GetPtr() const;

private:
    //=========================================================================
    // private variables.
    //=========================================================================
    ComPtr<ID3D12RootSignature>     m_RootSignature;

    //=========================================================================
    // private methods.
    //=========================================================================
    /* NOTHING */
};