#include <assert.h>
#include <atomic>         // std::atomic, std::memory_order_relaxed
#include <thread>         // std::thread

#ifndef DOCS_REFERENCECOUNTEDOBJECT_H
#define DOCS_REFERENCECOUNTEDOBJECT_H

namespace DSP
{
    template <typename ObjectType>
    struct ContainerDeletePolicy
    {
        static void destroy (ObjectType* object)
        {
            // If the line below triggers a compiler error, it means that you are using
            // an incomplete type for ObjectType (for example, a type that is declared
            // but not defined). This is a problem because then the following delete is
            // undefined behaviour. The purpose of the sizeof is to capture this situation.
            // If this was caused by a OwnedArray of a forward-declared type, move the
            // implementation of all methods trying to use the OwnedArray (e.g. the destructor
            // of the class owning it) into cpp files where they can see to the definition
            // of ObjectType. This should fix the error.
            ignoreUnused (sizeof (ObjectType));

            delete object;
        }
    };

    class ReferenceCountedObject
    {
    public:
        //==============================================================================
        /** Increments the object's reference count.
            This is done automatically by the smart pointer, but is public just
            in case it's needed for nefarious purposes.
        */
        void incReferenceCount() noexcept
        {
            ++refCount;
        }

        /** Decreases the object's reference count.
            If the count gets to zero, the object will be deleted.
        */
        void decReferenceCount() noexcept
        {
            assert (getReferenceCount() > 0);

            if (--refCount == 0)
                delete this;
        }

        /** Decreases the object's reference count.
            If the count gets to zero, the object will not be deleted, but this method
            will return true, allowing the caller to take care of deletion.
        */
        bool decReferenceCountWithoutDeleting() noexcept
        {
            assert (getReferenceCount() > 0);
            return --refCount == 0;
        }

        /** Returns the object's current reference count. */
        int getReferenceCount() const noexcept       { return refCount; }


    protected:
        //==============================================================================
        /** Creates the reference-counted object (with an initial ref count of zero). */
        ReferenceCountedObject() = default;

        /** Copying from another object does not affect this one's reference-count. */
        ReferenceCountedObject (const ReferenceCountedObject&) noexcept {}
        /** Copying from another object does not affect this one's reference-count. */
        ReferenceCountedObject (ReferenceCountedObject&&) noexcept {}
        /** Copying from another object does not affect this one's reference-count. */
        ReferenceCountedObject& operator= (const ReferenceCountedObject&) noexcept  { return *this; }
        /** Copying from another object does not affect this one's reference-count. */
        ReferenceCountedObject& operator= (ReferenceCountedObject&&) noexcept       { return *this; }

        /** Destructor. */
        virtual ~ReferenceCountedObject()
        {
            // it's dangerous to delete an object that's still referenced by something else!
            assert (getReferenceCount() == 0);
        }

        /** Resets the reference count to zero without deleting the object.
            You should probably never need to use this!
        */
        void resetReferenceCount() noexcept
        {
            refCount = 0;
        }

    private:
        //==============================================================================
        std::atomic<int> refCount { 0 };
        friend struct ContainerDeletePolicy<ReferenceCountedObject>;
    };
#endif // DOCS_REFERENCECOUNTEDOBJECT_H

    template <class ObjectType>
    class ReferenceCountedObjectPtr
    {
    public:
        /** The class being referenced by this pointer. */
        using ReferencedType = ObjectType;

        //==============================================================================
        /** Creates a pointer to a null object. */
        ReferenceCountedObjectPtr() = default;

        /** Creates a pointer to a null object. */
        ReferenceCountedObjectPtr (decltype (nullptr)) noexcept {}

        /** Creates a pointer to an object.
            This will increment the object's reference-count.
        */
        ReferenceCountedObjectPtr (ReferencedType* refCountedObject) noexcept
            : referencedObject (refCountedObject)
        {
            incIfNotNull (refCountedObject);
        }

        /** Creates a pointer to an object.
            This will increment the object's reference-count.
        */
        ReferenceCountedObjectPtr (ReferencedType& refCountedObject) noexcept
            : referencedObject (&refCountedObject)
        {
            refCountedObject.incReferenceCount();
        }

        /** Copies another pointer.
            This will increment the object's reference-count.
        */
        ReferenceCountedObjectPtr (const ReferenceCountedObjectPtr& other) noexcept
            : referencedObject (other.referencedObject)
        {
            incIfNotNull (referencedObject);
        }

        /** Takes-over the object from another pointer. */
        ReferenceCountedObjectPtr (ReferenceCountedObjectPtr&& other) noexcept
            : referencedObject (other.referencedObject)
        {
            other.referencedObject = nullptr;
        }

        /** Copies another pointer.
            This will increment the object's reference-count (if it is non-null).
        */
        template <typename Convertible>
        ReferenceCountedObjectPtr (const ReferenceCountedObjectPtr<Convertible>& other) noexcept
            : referencedObject (other.get())
        {
            incIfNotNull (referencedObject);
        }

        /** Changes this pointer to point at a different object.
            The reference count of the old object is decremented, and it might be
            deleted if it hits zero. The new object's count is incremented.
        */
        ReferenceCountedObjectPtr& operator= (const ReferenceCountedObjectPtr& other)
        {
            return operator= (other.referencedObject);
        }

        /** Changes this pointer to point at a different object.
            The reference count of the old object is decremented, and it might be
            deleted if it hits zero. The new object's count is incremented.
        */
        template <typename Convertible>
        ReferenceCountedObjectPtr& operator= (const ReferenceCountedObjectPtr<Convertible>& other)
        {
            return operator= (other.get());
        }

        /** Changes this pointer to point at a different object.
            The reference count of the old object is decremented, and it might be
            deleted if it hits zero. The new object's count is incremented.
        */
        ReferenceCountedObjectPtr& operator= (ReferencedType* newObject)
        {
            if (newObject != nullptr)
                return operator= (*newObject);

            reset();
            return *this;
        }

        /** Changes this pointer to point at a different object.
            The reference count of the old object is decremented, and it might be
            deleted if it hits zero. The new object's count is incremented.
        */
        ReferenceCountedObjectPtr& operator= (ReferencedType& newObject)
        {
            if (referencedObject != &newObject)
            {
                newObject.incReferenceCount();
                auto* oldObject = referencedObject;
                referencedObject = &newObject;
                decIfNotNull (oldObject);
            }

            return *this;
        }

        /** Resets this pointer to a null pointer. */
        ReferenceCountedObjectPtr& operator= (decltype (nullptr))
        {
            reset();
            return *this;
        }

        /** Takes-over the object from another pointer. */
        ReferenceCountedObjectPtr& operator= (ReferenceCountedObjectPtr&& other) noexcept
        {
            std::swap (referencedObject, other.referencedObject);
            return *this;
        }

        /** Destructor.
            This will decrement the object's reference-count, which will cause the
            object to be deleted when the ref-count hits zero.
        */
        ~ReferenceCountedObjectPtr()
        {
            decIfNotNull (referencedObject);
        }

        //==============================================================================
        /** Returns the object that this pointer references.
            The pointer returned may be null, of course.
        */
        ReferencedType* get() const noexcept                    { return referencedObject; }

        /** Resets this object to a null pointer. */
        void reset() noexcept
        {
            auto oldObject = referencedObject;  // need to null the pointer before deleting the object
            referencedObject = nullptr;         // in case this ptr is itself deleted as a side-effect
            decIfNotNull (oldObject);           // of the destructor
        }

        // the -> operator is called on the referenced object
        ReferencedType* operator->() const noexcept
        {
            assert (referencedObject != nullptr); // null pointer method call!
            return referencedObject;
        }

        /** Dereferences the object that this pointer references.
            The pointer returned may be null, of course.
        */
        ReferencedType& operator*() const noexcept              { assert (referencedObject != nullptr); return *referencedObject; }

        /** Checks whether this pointer is null */
        bool operator== (decltype (nullptr)) const noexcept     { return referencedObject == nullptr; }
        /** Checks whether this pointer is null */
        bool operator!= (decltype (nullptr)) const noexcept     { return referencedObject != nullptr; }

        /** Compares two ReferenceCountedObjectPtrs. */
        bool operator== (const ObjectType* other) const noexcept                 { return referencedObject == other; }
        /** Compares two ReferenceCountedObjectPtrs. */
        bool operator== (const ReferenceCountedObjectPtr& other) const noexcept  { return referencedObject == other.get(); }
        /** Compares two ReferenceCountedObjectPtrs. */
        bool operator!= (const ObjectType* other) const noexcept                 { return referencedObject != other; }
        /** Compares two ReferenceCountedObjectPtrs. */
        bool operator!= (const ReferenceCountedObjectPtr& other) const noexcept  { return referencedObject != other.get(); }

       #if JUCE_STRICT_REFCOUNTEDPOINTER
        /** Checks whether this pointer is null */
        explicit operator bool() const noexcept                 { return referencedObject != nullptr; }

       #else
        /** Returns the object that this pointer references.
            The pointer returned may be null, of course.
            Note that this methods allows the compiler to be very lenient with what it allows you to do
            with the pointer, it's safer to disable this by setting JUCE_STRICT_REFCOUNTEDPOINTER=1, which
            increased type safety and can prevent some common slip-ups.
        */
        operator ReferencedType*() const noexcept               { return referencedObject; }
       #endif

       #ifndef DOXYGEN
        [[deprecated ("Use the get method instead.")]]
        ReferencedType* getObject() const { return get(); }
       #endif

    private:
        //==============================================================================
        ReferencedType* referencedObject = nullptr;

        static void incIfNotNull (ReferencedType* o) noexcept
        {
            if (o != nullptr)
                o->incReferenceCount();
        }

        static void decIfNotNull (ReferencedType* o) noexcept
        {
            if (o != nullptr && o->decReferenceCountWithoutDeleting())
                ContainerDeletePolicy<ReferencedType>::destroy (o);
        }
    };
}
