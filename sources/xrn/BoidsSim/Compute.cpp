///////////////////////////////////////////////////////////////////////////
// Precompilled headers
///////////////////////////////////////////////////////////////////////////
#include <pch.hpp>

///////////////////////////////////////////////////////////////////////////
// Headers
///////////////////////////////////////////////////////////////////////////
#include <xrn/BoidsSim/Compute.hpp>



///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
// Constructors
//
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
::xrn::bsim::Compute::Compute(
    ::xrn::engine::vulkan::Device& device
)
    : m_device{ device }
{}



///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
// Rule of 5
//
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
::xrn::bsim::Compute::~Compute()
{
    // ::vkDestroyPipelineLayout(m_device.device(), m_pipelineLayout, nullptr);
}



///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
// Constructors
//
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
void ::xrn::bsim::Compute::run()
{
    // Create a buffer to store the data
    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = 1024 * sizeof(float);
    bufferInfo.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
    VkBuffer buffer;
    vkCreateBuffer(m_device.device(), &bufferInfo, nullptr, &buffer);

    // Allocate memory for the buffer
    VkMemoryRequirements memReq;
    vkGetBufferMemoryRequirements(m_device.device(), buffer, &memReq);
    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memReq.size;
    allocInfo.memoryTypeIndex = m_device.findMemoryType(memReq.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
    VkDeviceMemory memory;
    vkAllocateMemory(m_device.device(), &allocInfo, nullptr, &memory);
    vkBindBufferMemory(m_device.device(), buffer, memory, 0);

    // Fill the buffer with some initial data
    float* data;
    vkMapMemory(m_device.device(), memory, 0, memReq.size, 0, (void**)&data);
    for (int i = 0; i < 1024; i++) {
        data[i] = i;
    }
    vkUnmapMemory(m_device.device(), memory);

    // Create a descriptor set layout
    VkDescriptorSetLayoutBinding binding = {};
    binding.binding = 0;
    binding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    binding.descriptorCount = 1;
    binding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

    VkDescriptorSetLayoutCreateInfo layoutInfo = {};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = 1;
    layoutInfo.pBindings = &binding;
    VkDescriptorSetLayout descriptorSetLayout;
    vkCreateDescriptorSetLayout(m_device.device(), &layoutInfo, nullptr, &descriptorSetLayout);

    // Create a pipeline layout
    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;
    VkPipelineLayout pipelineLayout;
    vkCreatePipelineLayout(m_device.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout);

    // Create a compute pipeline
    VkShaderModule shaderModule;
    this->createShaderModule("shaders/Math/test.glsl.spv", shaderModule);
    VkPipelineShaderStageCreateInfo stageInfo = {};
    stageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    stageInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
    stageInfo.module = shaderModule;
    stageInfo.pName = "main";
    VkComputePipelineCreateInfo pipelineInfo = {};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    pipelineInfo.stage = stageInfo;
    pipelineInfo.layout = pipelineLayout;
    VkPipeline pipeline;
    vkCreateComputePipelines(m_device.device(), nullptr, 1, &pipelineInfo, nullptr, &pipeline);

    // Allocate a descriptor set
    // VkDescriptorSetAllocateInfo allocInfo2 = {};
    // allocInfo2.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    // allocInfo2.descriptorPool = descriptorPool;
    // allocInfo2.descriptorSetCount = 1;
    // allocInfo2.pSetLayouts = &descriptorSetLayout;
    // VkDescriptorSet descriptorSet;
    // vkAllocateDescriptorSets(m_device.device(), &allocInfo2, &descriptorSet);

    // Update the descriptor set
    // VkDescriptorBufferInfo bufferInfo = {};
    // bufferInfo.buffer = buffer;
    // bufferInfo.offset = 0;
    // bufferInfo.range = VK_WHOLE_SIZE;
    // VkWriteDescriptorSet writeDescriptorSet = {};
    // writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    // writeDescriptorSet.dstSet = descriptorSet;
    // writeDescriptorSet.dstBinding = 0;
    // writeDescriptorSet.descriptorCount = 1
}



///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
// Helpers
//
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
void ::xrn::bsim::Compute::createShaderModule(
    const ::std::string& filepath
    , ::VkShaderModule& shaderModule
)
{
    // open the file
    ::std::ifstream file{ filepath, ::std::ios::ate | ::std::ios::binary };
    if (!file.is_open()) {
        XRN_THROW("File '{}' has failed to open", filepath);
    }

    // check its size
    ::std::size_t fileSize{ static_cast<::std::size_t>(file.tellg()) };

    // get its content
    ::std::vector<::std::byte> code;
    code.resize(fileSize);
    file.seekg(0);
    file.read(::std::bit_cast<char*>(code.data()), fileSize);

    // create info shader module
    ::VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = ::std::bit_cast<const ::std::uint32_t*>(code.data());

    // create the shader module
    if (auto result{ ::vkCreateShaderModule(m_device.device(), &createInfo, nullptr, &shaderModule)}; result != VK_SUCCESS) {
        XRN_THROW("Failed to create Vulkan shader module (err: {})", ::xrn::engine::vulkan::errorToString(result));
    }
}
