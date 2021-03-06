#define _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_DEPRECATE

#include <string>
#include <vector>
#include <mshadow/tensor.h>
#include "./data.h"
#include "../global.h"
#include "../utils/utils.h"
#include "../utils/io.h"
#include "iter_mnist-inl.hpp"
#include "iter_augment_proc-inl.hpp"
#include "iter_batch_proc-inl.hpp"
#include "iter_mem_buffer-inl.hpp"
#include "iter_attach_txt-inl.hpp"
#include "iter_csv-inl.hpp"
#if CXXNET_USE_OPENCV
#include "iter_thread_imbin-inl.hpp"
#include "iter_thread_imbin_x-inl.hpp"
#include "iter_img-inl.hpp"
#endif

namespace cxxnet {
IIterator<DataBatch> *CreateIterator(const std::vector< std::pair<std::string, std::string> > &cfg) {
  size_t i = 0;
  IIterator<DataBatch> *it = NULL;
  for (; i < cfg.size(); ++i) {
    const char *name = cfg[i].first.c_str();
    const char *val  = cfg[i].second.c_str();
    if (!strcmp(name, "iter")) {
      if (!strcmp(val, "mnist")) {
        utils::Check(it == NULL, "mnist can not chain over other iterator");
        it = new MNISTIterator(); continue;
      }
      #if CXXNET_USE_OPENCV
      if (!strcmp(val, "imgbinold")) {
        utils::Assert(it == NULL, "image binary can not chain over other iterator");
        it = new BatchAdaptIterator(new AugmentIterator(new ThreadImagePageIterator()));
        continue;
      }
      // redirect all io to new iterator
      if (!strcmp(val, "imgbinx") || !strcmp(val, "imgbin")) {
        utils::Assert(it == NULL, "image binary can not chain over other iterator");
        it = new BatchAdaptIterator(new AugmentIterator(new ThreadImagePageIteratorX()));
        continue;
      }
      if (!strcmp(val, "img")) {
        utils::Assert(it == NULL, "image list iterator can not chain over other iterator");
        it = new BatchAdaptIterator(new AugmentIterator(new ImageIterator()));
        continue;
      }
      #endif
      if (!strcmp(val, "threadbuffer")) {
        utils::Assert(it != NULL, "must specify input of threadbuffer");
        it = new ThreadBufferIterator(it);
        continue;
      }
      if (!strcmp(val, "membuffer")) {
        utils::Assert(it != NULL, "must specify input of memory buffer");
        it = new DenseBufferIterator(it);
        continue;
      }
      if (!strcmp(val, "attachtxt")) {
        utils::Assert(it != NULL, "must specify input of attach txt buffer");
        it = new AttachTxtIterator(it);
        continue;
      }
      if (!strcmp(val, "csv")) {
        utils::Assert(it == NULL, "csv iter cannot chain over other iterator.");
        it = new BatchAdaptIterator(new CSVIterator());
        continue;
      }
      utils::Error("unknown iterator type %s", val);
    }
    if (it != NULL) {
      it->SetParam(name, val);
    }
  }
  utils::Assert(it != NULL, "must specify iterator by iter=itername");
  return it;
}
} // namespace cxxnet
