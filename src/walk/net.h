// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// net.h - Nerual network and gradient decent
#pragma once
#include "lib/math.h"
#include "lib/os_main.h"
#include "lib/rand.h"

typedef enum {
    Activation_None,
    Activation_Sigmoid,
    Activation_Relu,
} Net_Activation;

// A -> B
typedef struct Net_Layer Net_Layer;
struct Net_Layer {
    // Number of output nodes in this layer
    u32 output_count;
    Net_Activation activation_function;

    f32 *weight;     // [OUTPUT][INPUT]
    f32 *bias;       // [OUTPUT]
    f32 *activation; // [OUTPUT]

    f32 *weight_gradient;
    f32 *bias_gradient;
    f32 *activation_gradient;

    // List links in a neural network
    Net_Layer *next;
    Net_Layer *prev;
};

typedef struct {
    // Linked list
    Net_Layer *layer_first;
    Net_Layer *layer_last;
} Net_Network;

// A0 -> W -> S -> A1
// A1 -> W -> S -> A2
//
// L: Left hand side / Input
// W: Weight matrix, transforming L to S
// S: Result of L*W
// A: Activation act(S)
// R: Right hand side / Output becomes A

// Sum -> Output
static f32 act_fwd(Net_Activation act, f32 sum) {
    switch (act) {
    case Activation_None:
        return sum;
    case Activation_Relu:
        return sum > 0 ? sum : 0.01 * sum;
    case Activation_Sigmoid:
        return 1.0f / (1.0f + f_exp(-sum));
    }
}

// Y -> dY/dX
// Output-> dOutput / dSum
static f32 act_drv(Net_Activation act, f32 output) {
    switch (act) {
    case Activation_None:
        return 1;
    case Activation_Relu:
        return output > 0 ? 1 : 0.01;
    case Activation_Sigmoid:
        return output * (1 - output);
    }
}

// forward pass
// reads input_activation
// writes output_activation
static void net_layer_forward(const Net_Layer *input, Net_Layer *output) {
    u32 input_count = input->output_count;
    u32 output_count = output->output_count;
    for (u32 output_ix = 0; output_ix < output_count; ++output_ix) {
        f32 sum = 0;
        for (u32 input_ix = 0; input_ix < input_count; ++input_ix) {
            f32 weight = output->weight[output_ix * input_count + input_ix];
            f32 value = input->activation[input_ix];
            sum += value * weight;
        }
        sum += output->bias[output_ix];
        output->activation[output_ix] = act_fwd(output->activation_function, sum);
    }
}

static void nn_mul_fwd(f32 *weight, u32 input_count, f32 *input, u32 output_count, f32 *output) {
    for (u32 output_ix = 0; output_ix < output_count; ++output_ix) {
        f32 sum = 0;
        for (u32 input_ix = 0; input_ix < input_count; ++input_ix) {
            sum += input[input_ix] * weight[output_ix * input_count + input_ix];
        }
        output[output_ix] += sum;
    }
}

static void nn_relu(u32 count, f32 *x, f32 *y, f32 *d_yx) {
    for (u32 i = 0; i < count; ++i) {
        f32 slope = x[i] > 0 ? 1 : 0.01;
        y[i] = x[i] * slope;
        d_yx[i] = slope;
    }
}

// Backwards Pass A <- B
// reads input_activation
// reads output_activation
// reads output_gradient
// writes input_gradient
// modifies weights and bias
static void net_layer_backward(Net_Layer *input, Net_Layer *output) {
    u32 input_count = input->output_count;
    u32 output_count = output->output_count;

    std_memzero((u8 *)input->activation_gradient, sizeof(f32) * input_count);

    for (u32 output_ix = 0; output_ix < output_count; ++output_ix) {
        f32 value = output->activation[output_ix];

        // How does the final error change baed on the node output? (activation)
        f32 d_err_act = output->activation_gradient[output_ix];

        // How does the activation change based on the sum?
        f32 d_act_sum = act_drv(output->activation_function, value);

        // Combine
        f32 d_err_sum = d_err_act * d_act_sum;

        for (u32 input_ix = 0; input_ix < input_count; ++input_ix) {
            u32 weight_ix = output_ix * input_count + input_ix;
            f32 weight = output->weight[weight_ix];
            f32 input_value = input->activation[input_ix];

            // How does the sum change based on the weight
            // -> linear relationship with input
            f32 d_sum_weight = input_value;

            // How does the sum change based on the Input?
            // -> linear relationship with weight
            f32 d_sum_input = weight;

            // Compute effect on final error value
            f32 d_err_weight = d_err_sum * d_sum_weight;
            f32 d_err_input = d_err_sum * d_sum_input;

            // Apply weight gradient
            output->weight_gradient[weight_ix] += d_err_weight;

            // Propagate input gradient
            input->activation_gradient[input_ix] += d_err_input;
        }

        // How does the sum change based on the bias?
        f32 d_sum_bias = 1;
        f32 d_err_bias = d_err_sum * d_sum_bias;

        // Apply Bias gradient
        output->bias_gradient[output_ix] += d_err_bias;
    }
}

// Apply computed gradient after a number of iterations
static void net_layer_update(const Net_Layer *input, Net_Layer *output, f32 rate) {
    u32 input_count = input->output_count;
    u32 output_count = output->output_count;

    // Weights
    for (u32 i = 0; i < output_count * input_count; ++i) {
        output->weight[i] -= output->weight_gradient[i] * rate;
        output->weight_gradient[i] = 0;
    }

    // Bias
    for (u32 i = 0; i < output_count; ++i) {
        output->bias[i] -= output->bias_gradient[i] * rate;
        output->bias_gradient[i] = 0;
    }
}

static void net_update(Net_Network *network, f32 rate) {
    Net_Layer *input = network->layer_first;
    Net_Layer *output = input->next;
    while (output) {
        net_layer_update(input, output, rate);
        input = output;
        output = output->next;
    }
}

// Full forward pass
static void net_forward(Net_Network *network) {
    Net_Layer *input = network->layer_first;
    Net_Layer *output = input->next;
    while (output) {
        net_layer_forward(input, output);
        input = output;
        output = output->next;
    }
}

// Full backward pass, updating weight and bias gradients
static void net_backward(Net_Network *network) {
    Net_Layer *output = network->layer_last;
    Net_Layer *input = output->prev;
    while (input) {
        net_layer_backward(input, output);
        output = input;
        input = input->prev;
    }
}

// Compute output gradient based on output activation and target activation
// returns total error
static f32 net_layer_compute_output_gradient(Net_Layer *output, f32 *target) {
    f32 error_total = 0;
    for (u32 i = 0; i < output->output_count; ++i) {
        f32 x_output = output->activation[i];
        f32 x_target = target[i];
        f32 diff = x_output - x_target;
        f32 error = diff * diff;
        f32 grad = 2 * (x_output - x_target);
        output->activation_gradient[i] = grad;

        // Statistics
        error_total += error;
    }
    return error_total;
}

// Compute output gradient based on output activation and target activation
// returns total error
static f32 net_compute_output_gradient(Net_Network *network, f32 *target) {
    return net_layer_compute_output_gradient(network->layer_last, target);
}

static Net_Layer *net_layer_new(Memory *mem, Rand *rand, u32 input_count, u32 output_count, Net_Activation act_fcn) {
    Net_Layer *layer = mem_struct(mem, Net_Layer);
    layer->activation_function = act_fcn;
    layer->output_count = output_count;
    layer->activation = mem_array_zero(mem, f32, output_count);
    layer->activation_gradient = mem_array_zero(mem, f32, output_count);

    // No inputs means it is an input layer
    if (input_count) {
        layer->weight = mem_array_zero(mem, f32, output_count * input_count);
        layer->bias = mem_array_zero(mem, f32, output_count);

        // Accumulators
        layer->weight_gradient = mem_array_zero(mem, f32, output_count * input_count);
        layer->bias_gradient = mem_array_zero(mem, f32, output_count);

        if (act_fcn == Activation_Relu) {
            // He initialization for ReLU
            f32 limit = f_sqrt(2.0f / input_count);
            for (u32 i = 0; i < output_count * input_count; ++i) {
                layer->weight[i] = rand_f32(rand, -limit, limit);
            }

            // Bias can be initialized to 0 or a small value to prevent dying ReLU
            for (u32 i = 0; i < output_count; ++i) {
                layer->bias[i] = 0.01;
            }
        } else {
            for (u32 i = 0; i < output_count * input_count; ++i) {
                layer->weight[i] = rand_f32(rand, -1, 1);
            }

            for (u32 i = 0; i < output_count; ++i) {
                layer->bias[i] = 0.0;
            }
        }
    }

    return layer;
}

static Net_Layer *net_push_layer(Net_Network *net, Memory *mem, u32 output_count, Net_Activation act_fcn) {
    Net_Layer *input = net->layer_last;

    if (!input) {
        // Add input layer
        Net_Layer *layer = net_layer_new(mem, G->rand, 0, output_count, act_fcn);
        net->layer_first = layer;
        net->layer_last = layer;
        return layer;
    } else {
        Net_Layer *layer = net_layer_new(mem, G->rand, input->output_count, output_count, act_fcn);

        // Append to network
        input->next = layer;
        layer->prev = input;
        net->layer_last = layer;
        return layer;
    }
}

static void net_test(void) {
    // Create nerual network
    Net_Network *network = mem_struct(G->mem, Net_Network);
    net_push_layer(network, G->mem, 2, Activation_None);
    net_push_layer(network, G->mem, 4, Activation_Relu);
    net_push_layer(network, G->mem, 4, Activation_Relu);
    net_push_layer(network, G->mem, 1, Activation_Sigmoid);

    u32 batch_size = 64 * 10;
    u32 iteration_count = 8000;
    f32 error = 0;
    for (u32 j = 0; j < iteration_count; ++j) {
        error = 0;
        for (u32 i = 0; i < batch_size; ++i) {
            u32 x = rand_u32(G->rand, 0, 2);
            u32 y = rand_u32(G->rand, 0, 2);
            u32 o = x != y;

            // Write input
            network->layer_first->activation[0] = x;
            network->layer_first->activation[1] = y;
            f32 target[1] = {o};

            net_forward(network);
            error += net_compute_output_gradient(network, target);
            net_backward(network);
        }
        net_update(network, 0.1);
        error /= batch_size;

        if(j % 1000 == 0) {
            fmt_s(G->fmt, "Error: ");
            fmt_f(G->fmt, error);
            fmt_s(G->fmt, "\n");
        }
    }
    assert0(f_abs(error) < 0.01);
}
