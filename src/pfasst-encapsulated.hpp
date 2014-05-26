/*
 * Host based encapsulated base sweeper.
 */

#ifndef _PFASST_ENCAPSULATED_HPP_
#define _PFASST_ENCAPSULATED_HPP_

#include <algorithm>
#include <vector>

#include "pfasst-interfaces.hpp"
#include "pfasst-quadrature.hpp"

using namespace std;

namespace pfasst { namespace encap {

  typedef enum EncapType { solution, function } EncapType;

  //
  // encapsulation
  //

  class Encapsulation {
  public:
    virtual ~Encapsulation() { }

    // required for interp/restrict helpers
    virtual void interpolate(const Encapsulation *) {
      throw NotImplementedYet("mlsdc/pfasst");
    }
    virtual void restrict(const Encapsulation *) {
      throw NotImplementedYet("mlsdc/pfasst");
    }

    // required for time-parallel communications
    virtual unsigned int nbytes() {
      throw NotImplementedYet("pfasst");
    }
    virtual void send() {
      throw NotImplementedYet("pfasst");
    }
    virtual void recv() {
      throw NotImplementedYet("pfasst");
    }

    // required for host based encap helpers
    virtual void setval(double) {
      throw NotImplementedYet("encap");
    }
    virtual void copy(const Encapsulation *) {
      throw NotImplementedYet("encap");
    }
    virtual void saxpy(double a, const Encapsulation *) {
      throw NotImplementedYet("encap");
    }
    // virtual void mat_apply(Encapsulation dst[], double a, matrix m,
    // 			   const Encapsulation src[]) {
    //   throw NotImplementedYet("encap");
    // }
  };

  class EncapsulationFactory {
  public:
    virtual Encapsulation* create(const EncapType) = 0;
  };

  template<typename T>
  class EncapsulatedSweeperMixin : public ISweeper {
    vector<T>                        nodes;
    shared_ptr<EncapsulationFactory> factory;

  public:
    void set_nodes(vector<T> nodes) {
      this->nodes = nodes;
    }
    const vector<T> get_nodes() const {
      return nodes;
    }

    void set_factory(EncapsulationFactory* factory) {
      this->factory = shared_ptr<EncapsulationFactory>(factory);
    }
    EncapsulationFactory* get_factory() const {
      return factory.get();
    }

    virtual void set_q0(const Encapsulation* q0) {
      throw NotImplementedYet("sweeper");
    }
    virtual Encapsulation* get_qend() {
      throw NotImplementedYet("sweeper");
      return NULL;
    }

    virtual void advance() {
      throw NotImplementedYet("sweeper");
    }
  };

  template<class T>
  class PolyInterpMixin : public T {
    virtual void interpolate(const ISweeper*) { }
    virtual void restrict(const ISweeper*) { }
  };

  template<typename scalar>
  struct VectorEncapsulation : public vector<scalar>, public Encapsulation {
    VectorEncapsulation(int size) : vector<scalar>(size) { }
    virtual unsigned int nbytes() const {
      return sizeof(scalar) * this->size();
    }
    void setval(scalar v) {
      for (int i=0; i<this->size(); i++)
	(*this)[i] = v;
    }
    void copy(const Encapsulation* X) {
      const auto* x = dynamic_cast<const VectorEncapsulation*>(X);
      for (int i=0; i<this->size(); i++)
	(*this)[i] = (*x)[i];
    }
    void saxpy(double a, const Encapsulation *X) {
      const auto* x = dynamic_cast<const VectorEncapsulation*>(X);
      for (int i=0; i<this->size(); i++)
	(*this)[i] += a * (*x)[i];
    }
  };

  template<typename T>
  class VectorFactory : public EncapsulationFactory {
    int size;
  public:
    int dofs() { return size; }
    VectorFactory(const int size) : size(size) { }
    Encapsulation* create(const EncapType) {
      return new VectorEncapsulation<T>(size);
    }
  };

  } }

#endif
