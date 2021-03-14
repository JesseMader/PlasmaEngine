// MIT Licensed (see LICENSE.md).
#pragma once

namespace Plasma
{

/// Describes a pipeline of tools plus a backend for shader translation. The
/// simple shader generator will convert lightning shaders to spir-v binary, run
/// this binary through the tools, then finally pass it to the backend to get a
/// final shader result.
class ShaderPipelineDescription
{
public:
  typedef Lightning::Ref<LightningShaderIRTranslationPass> TranslationPassRef;
  typedef Lightning::Ref<LightningShaderIRBackend> BackendPassRef;

  Array<TranslationPassRef> mToolPasses;
  /// Debug passes run after all of the tool passes. These are not fed
  /// into the backend, but are used to collect any debug information
  /// (such as the validator) on the final spir-v data.
  Array<TranslationPassRef> mDebugPasses;
  BackendPassRef mBackend;

  LightningRefLink(ShaderPipelineDescription);
};

/// Simplified reflection data generated by the simple shader generator. As
/// there can be many passes in a pipeline it can be quite complicated to figure
/// out what where fragment properties actually end up in a final backend's
/// binding points. This structure builds a quick-mapping from shader + fragment
/// + property to final uniform/sampledimage reflection information. This class
/// is meant to be usable for most project to generate final reflection data. If
/// a more complicated shader generation scheme is used then this should also
/// serve as an aid to understand how reflection data is transfered through a
/// pipeline.
class SimplifiedShaderReflectionData
{
public:
  typedef LightningShaderIRCompositor::ShaderStageDescription ShaderStageDescription;
  typedef ShaderStageInterfaceReflection::SampledImageRemappings SampledImageRemappings;
  typedef Lightning::Ref<ShaderTranslationPassResult> PassResultRef;
  LightningRefLink(SimplifiedShaderReflectionData);

  /// Describes what uniform buffer a property ends up in and where.
  struct UniformReflectionData
  {
    /// The index into the stage reflection data's uniform buffers.
    size_t mBufferIndex;
    /// The index within the uniform buffer that this member maps to.
    size_t mMemberIndex;
  };

  /// For a sampler, image, or sampled image, defines all resultant samplers,
  /// images, and sampled images that this results in. The indices are used to
  /// map into the stage reflection data. Note: this is a multi-to-multi mapping
  /// as any stage can split or merge some of these types together.
  struct SampledImageRemappingData
  {
    Array<size_t> mSampledImageIds;
    Array<size_t> mImageIds;
    Array<size_t> mSamplerIds;
  };

  /// Describes how to find the reflection data for a ssbo.
  struct StructuredStorageBufferRemappingData
  {
    /// The index into the reflection data of the final stage.
    size_t mIndex;
  };

  /// Describes how to find the reflection data for a storage image.
  struct StorageImageRemappingData
  {
    /// The index into the reflection data of the final stage.
    size_t mIndex;
  };

  /// Lookup data for a fragment. Contains information about how to turn a
  /// property into a location in the overall stage reflection data.
  struct FragmentLookup
  {
    /// Map of properties (by name) to locations with a uniform buffer.
    HashMap<String, UniformReflectionData> mMaterialUniforms;
    /// Map of properties (by name) of sampled images. This can turn into any
    /// number of samplers, images, and sampled images.
    HashMap<String, SampledImageRemappingData> mSampledImages;
    /// Map of properties (by name) of samplers. This can turn into any number
    /// of samplers and images.
    HashMap<String, SampledImageRemappingData> mSamplers;
    /// Map of properties (by name) of images. This can turn into any number of
    /// images and sampled images.
    HashMap<String, SampledImageRemappingData> mImages;
    /// Map of properties (by name) of storage images to their reflection data
    /// index.
    HashMap<String, StorageImageRemappingData> mStorageImages;
    /// Map of properties (by name) of structured storage buffers to their
    /// reflection data index.
    HashMap<String, StructuredStorageBufferRemappingData> mStructedStorageBuffers;
  };

  /// Build the cached reflection data for quick access for a shader.
  /// This will walk all of the results from the passes and 'merge' them
  /// together to get a quick jump table.
  void CreateReflectionData(LightningShaderIRLibrary* shaderLibrary,
                            ShaderStageDescription& stageDef,
                            Array<PassResultRef>& passResults);

  /// Find the reflection data for a uniform given the fragment and property.
  /// Returns null if the property can't be found.
  const ShaderResourceReflectionData* FindUniformReflectionData(const LightningShaderIRType* fragmentType, StringParam propertyName) const;
  /// Finds all potential images, samplers, and sampled images that the given
  /// SampledImage property results in.
  void FindSampledImageReflectionData(const LightningShaderIRType* fragmentType,
                                      StringParam propertyName,
                                      Array<const ShaderResourceReflectionData*>& results) const;
  /// Finds all potential images and sampled images that the given Image
  /// property results in.
  void FindImageReflectionData(const LightningShaderIRType* fragmentType,
                               StringParam propertyName,
                               Array<const ShaderResourceReflectionData*>& results) const;
  /// Finds all potential samplers and sampled images that the given Sampler
  /// property results in.
  void FindSamplerReflectionData(const LightningShaderIRType* fragmentType,
                                 StringParam propertyName,
                                 Array<const ShaderResourceReflectionData*>& results) const;
  /// Find the reflection data for a storage image given the fragment and
  /// property. Returns null if the property can't be found.
  const ShaderResourceReflectionData* FindStorageImage(const LightningShaderIRType* fragmentType, StringParam propertyName) const;
  /// Find the reflection data for a structed storage buffer given the fragment
  /// and property. Returns null if the property can't be found.
  const ShaderResourceReflectionData* FindStructedStorageBuffer(const LightningShaderIRType* fragmentType, StringParam propertyName) const;

  /// Map lookup of fragment name to property information about the fragment.
  HashMap<String, FragmentLookup> mFragmentLookup;
  /// The final reflection data of this shader. Contains all descriptions about
  /// interface buffers, uniforms, etc...
  ShaderStageInterfaceReflection mReflection;

private:
  typedef HashMap<String, size_t> NameToIndexMap;

  // Internal helper class used to remap uniform buffers between stages to each
  // other.
  struct SimpleResourceRemappingData
  {
    size_t mIndex;
    String mName;
    bool mActive;
  };

  // Creates the final mapped data for uniform buffers.
  void CreateUniformReflectionData(LightningShaderIRLibrary* shaderLibrary,
                                   ShaderStageDescription& stageDef,
                                   Array<PassResultRef>& passResults);

  // Creates the final mapped data for samplers, images, and sampled images.
  void CreateSamplerAndImageReflectionData(LightningShaderIRLibrary* shaderLibrary,
                                           ShaderStageDescription& stageDef,
                                           Array<PassResultRef>& passResults);
  // Given an array of pass results, this recursively walks from the current
  // pass following where all of the input mappings end up at and puts the
  // results in the output mappings.
  void RecursivelyBuildSamplerAndImageMappings(Array<PassResultRef>& passResults,
                                               size_t passIndex,
                                               SampledImageRemappings& inputMappings,
                                               SampledImageRemappings& outputMappings);
  // Merges the source image mappings into the destination image mappings.
  void MergeRemappings(SampledImageRemappings& dest, SampledImageRemappings& source);
  /// Convert the final name mappings to actual indices in the reflection data.
  void BuildFinalSampledImageMappings(SampledImageRemappings* resourceMappings,
                                      NameToIndexMap& samplerIndices,
                                      NameToIndexMap& imageIndices,
                                      NameToIndexMap& sampledImageIndices,
                                      SampledImageRemappingData& results);

  /// Fills out information for an individual search map (e.g. sampler/image)
  /// given the property name.
  void PopulateSamplerAndImageData(HashMap<String, SampledImageRemappingData>& searchMap,
                                   StringParam propertyName,
                                   Array<const ShaderResourceReflectionData*>& results) const;

  /// Create reflectiond ata for simple opaque types (e.g. storage image and
  /// ssbos).
  void CreateSimpleOpaqueTypeReflectionData(LightningShaderIRLibrary* shaderLibrary,
                                            ShaderStageDescription& stageDef,
                                            Array<PassResultRef>& passResults);
};

/// This class is basically a helper wrapper around projects, libraries, the
/// compositor, and the translator. This should give simple examples of how to
/// use the lightning shader translation system.
class SimpleLightningShaderIRGenerator : public Lightning::EventHandler
{
public:
  typedef LightningSpirVFrontEnd FrontEndTranslatorType;
  typedef Lightning::Ref<ShaderTranslationPassResult> TranslationPassResultRef;
  typedef Lightning::Ref<SimplifiedShaderReflectionData> SimplifiedReflectionRef;

  SimpleLightningShaderIRGenerator(FrontEndTranslatorType* frontEndTranslator);
  SimpleLightningShaderIRGenerator(FrontEndTranslatorType* frontEndTranslator, LightningShaderSpirVSettings* settings);
  ~SimpleLightningShaderIRGenerator();

  /// Setup all dependent libraries that we have and compile them into a shader
  /// library (only once) for all subsequent fragment and shader operations.
  /// This is not done in the constructor so the user has a chance to connect
  /// events to this before the dependent libraries are built (in-case any
  /// errors happen).
  void SetupDependencies(StringParam extensionsDirectoryPath);
  /// Stores a pipeline description to be used for shader translation.
  void SetPipeline(ShaderPipelineDescription* pipeline);
  /// Clear all projects and reset the translator (clears the library
  /// translation)
  void ClearAll();

  void AddFragmentCode(StringParam fragmentCode, StringParam fileName, void* userData);
  /// Compiles fragments and translates them to an internal representation
  /// (spir-v)
  bool CompileAndTranslateFragments();
  void ClearFragmentsProjectAndLibrary();

  /// Composes the given shader definition of fragments together. Stores the
  /// resultant lightning shader code in the given definition object. The lightning
  /// shader code is also cached internally.
  bool ComposeShader(LightningShaderIRCompositor::ShaderDefinition& shaderDef, ShaderCapabilities& capabilities);

  /// Composes the given shader definition of compute fragments. Stores the resultant
  /// lightning shader code in the given definition object. The lightning shader code is also cached internally.
  /// Compute properties (such as the local workgroup size) should be passed through (can optionally be
  /// null to use the first fragment's properties). 
  bool ComposeComputeShader(LightningShaderIRCompositor::ShaderDefinition& shaderDef, ShaderCapabilities& capabilities, LightningShaderIRCompositor::ComputeShaderProperties* computeProperties = nullptr);
  
  void AddShaderCode(StringParam shaderCode, StringParam fileName, void* userData);
  /// Compiles shaders and translates them to an internal representation
  /// (spir-v)
  bool CompileAndTranslateShaders();
  void ClearShadersProjectAndLibrary();

  // compilation.
  // Compiles the shader library through a pipeline of tools to a final backend.

  /// Compiles the current shader library (CompileAndTranslateShaders must be
  /// called first) using the pre-set shader pipeline.
  bool CompilePipeline();
  // Compiles the current shader library (CompileAndTranslateShaders must be
  // called first) given a pipeline.
  bool CompilePipeline(ShaderPipelineDescription& pipeline);

  // Interface
  /// Find a the shader type for a fragment by name. Only checks the fragment
  /// library.
  LightningShaderIRType* FindFragmentType(StringParam typeName);
  /// Find a the shader type for a shader by name. Only checks the shader
  /// library.
  LightningShaderIRType* FindShaderType(StringParam typeName);
  /// Finds the translation result (byte data + reflection data) for a shader.
  ShaderTranslationPassResult* FindTranslationResult(const LightningShaderIRType* shaderType);
  /// Finds the cached simplified reflection data for a shader.
  SimplifiedShaderReflectionData* FindSimplifiedReflectionResult(const LightningShaderIRType* shaderType);

  /// Load default name settings
  static void LoadNameSettings(SpirVNameSettings& nameSettings);
  /// Create the settings that the unit tests use.
  static LightningShaderSpirVSettings* CreateUnitTestSettings(SpirVNameSettings& nameSettings);
  /// Create the settings that plasma uses.
  static LightningShaderSpirVSettings* CreatePlasmaSettings(SpirVNameSettings& nameSettings);

  void Initialize(FrontEndTranslatorType* frontEndTranslator, LightningShaderSpirVSettings* settings);
  void SetupEventConnections();
  // Helper to setup a lot of shared data on the translator
  void SetupTranslator(FrontEndTranslatorType* translator);

  /// Compiles a shader stage with a given pipeline. Caches the results
  /// internally. Also builds the simplified reflection data for this shader.
  bool CompilePipeline(LightningShaderIRCompositor::ShaderStageDescription& stageDef, ShaderPipelineDescription& pipeline);
  /// Runs the pipeline on one shader type and fills out all of the results for
  /// each translation pass. Also fills out an array of debug results for all of
  /// the debug passes in the pipeline. A debug pass will run on the last pass
  /// (before the backend) and typically is used for the validator or
  /// disassembler.
  bool CompilePipeline(LightningShaderIRType* shaderType,
                       ShaderPipelineDescription& pipeline,
                       Array<TranslationPassResultRef>& pipelineResults,
                       Array<TranslationPassResultRef>& debugResults);

  void RecursivelyLoadDirectory(StringParam path, LightningShaderIRProject& project);

  // Basic handler to forward the errors from the underlying
  // project/translator to whoever is listening on this class.
  void OnForwardEvent(Lightning::EventData* e);

  // The shared settings to be used through all of the compositing/translation
  LightningShaderSpirVSettingsRef mSettings;
  // The front-end translator used.
  FrontEndTranslatorType* mFrontEndTranslator;

  // Core libraries built once.
  LightningShaderIRLibraryRef mCoreLibrary;
  LightningShaderIRLibraryRef mShaderIntrinsicsLibrary;
  LightningShaderIRLibraryRef mExtensionsLibraryRef;

  // The individual fragments to be assembled into shaders
  LightningShaderIRProject mFragmentProject;
  LightningShaderIRLibraryRef mFragmentLibraryRef;

  // The shaders built on-top of fragments
  LightningShaderIRProject mShaderProject;
  LightningShaderIRLibraryRef mShaderLibraryRef;

  typedef HashMap<String, LightningShaderIRCompositor::ShaderDefinition> ShaderDefinitionMap;
  /// Keep a mapping of shader name to definition so that we can lookup
  /// the actual shaders for each shader stage (e.g. vertex/pixel).
  ShaderDefinitionMap mShaderDefinitionMap;

  /// Cached translation result for a pipeline run.
  struct ShaderTranslationResult
  {
    /// The final results from running a pipeline.
    TranslationPassResultRef mFinalPassData;
    /// The simplified reflection data.
    SimplifiedReflectionRef mReflectionData;

    /// Maps 1-1 in-order of the debug passes on the pipeline.
    /// Mostly used to collect disassembly and validator results.
    Array<TranslationPassResultRef> mDebugResults;
  };

  /// Map of shaders by name (shader type name) to their translation results.
  HashMap<String, ShaderTranslationResult> mShaderResults;
  /// A pipeline description set by the user to be run on all shader
  /// translations.
  Lightning::Ref<ShaderPipelineDescription> mPipeline;
};

} // namespace Plasma
