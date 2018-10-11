// ----------------------------------------------------------------------------
//  OgmaNeo
//  Copyright(c) 2016-2018 Ogma Intelligent Systems Corp. All rights reserved.
//
//  This copy of OgmaNeo is licensed to you under the terms described
//  in the OGMANEO_LICENSE.md file included in this distribution.
// ----------------------------------------------------------------------------

#pragma once

#include "Helpers.h"

namespace ogmaneo {
    /*!
    \brief A 2D prediction layer
    Predicts the targets one timestep ahead of time
    */
    class Actor {
    public:
        /*!
        \brief Visible layer descriptor
        */
        struct VisibleLayerDesc {
            /*!
            \brief Visible layer size
            */
            cl_int3 _size;

            /*!
            \brief Radius onto hidden layer
            */
            cl_int _radius;
            /*!
            \brief Initialize defaults
            */
            VisibleLayerDesc()
                : _size({ 4, 4, 16 }),
                _radius(2)
            {}
        };

        /*!
        \brief Visible layer
        */
        struct VisibleLayer {
            //!@{
            /*!
            \brief Visible layer values and buffers
            */
            cl::Buffer _visibleCs;

            cl::Buffer _weights;

            cl_float2 _hiddenToVisible;
            //!@}
        };

    private:
        /*!
        \brief Size of the hidden layer (output)
        */
        cl_int3 _hiddenSize;

        //!@{
        /*!
        \brief Buffers
        */
        cl::Buffer _hiddenCs;

        DoubleBuffer _hiddenActivations;
        //!@}

        //!@{
        /*!
        \brief Visible layers and associated descriptors
        */
        std::vector<VisibleLayer> _visibleLayers;
        std::vector<VisibleLayerDesc> _visibleLayerDescs;
        //!@}

        //!@{
        /*!
        \brief Kernels
        */
        cl::Kernel _forwardKernel;
        cl::Kernel _inhibitKernel;
        cl::Kernel _learnKernel;
        //!@}

    public:
        /*!
        \brief Learning rate
        */
        cl_float _alpha;

        /*!
        \brief Discount factor
        */
        cl_float _gamma;

        /*!
        \brief Initialize defaults
        */
        Actor()
        : _alpha(0.01f), _gamma(0.99f)
        {}

        /*!
        \brief Create an actor layer with random initialization
        \param cs is the ComputeSystem
        \param prog is the ComputeProgram associated with the ComputeSystem and loaded with the actor kernel code
        \param hiddenSize size of the predictions (output)
        \param visibleLayerDescs are descriptors for visible layers
        \param rng a random number generator
        */
        void createRandom(ComputeSystem &cs, ComputeProgram &prog,
            cl_int3 hiddenSize, const std::vector<VisibleLayerDesc> &visibleLayerDescs,
            std::mt19937 &rng);

        /*!
        \brief Activate the actor (predict values)
        \param cs is the ComputeSystem
        \param visibleCs the visible (input) layer states
        \param targetCs target hidden activations to predict (target hidden state/actions)
        \param reward reinforcment signal
        \param learn whether to learn
        */
        void step(ComputeSystem &cs, const std::vector<cl::Buffer> &visibleCs, const cl::Buffer &targetCs, float reward, bool learn);

        /*!
        \brief Get number of visible layers
        */
        size_t getNumVisibleLayers() const {
            return _visibleLayers.size();
        }

        /*!
        \brief Get a visible layer
        */
        const VisibleLayer &getVisibleLayer(int index) const {
            return _visibleLayers[index];
        }

        /*!
        \brief Get a visible layer descriptor
        */
        const VisibleLayerDesc &getVisibleLayerDesc(int index) const {
            return _visibleLayerDescs[index];
        }

        /*!
        \brief Get the hidden activations (predictions)
        */
        const cl::Buffer &getHiddenCs() const {
            return _hiddenCs;
        }

        /*!
        \brief Get the hidden size
        */
        cl_int3 getHiddenSize() const {
            return _hiddenSize;
        }

        /*!
        \brief Get the weights for a visible layer
        */
        const cl::Buffer &getWeights(int v) {
            return _visibleLayers[v]._weights;
        }
    };
}